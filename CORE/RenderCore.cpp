#include "RenderCore.h"

//使用智能指针ComPtr
using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;

float RenderCore::AspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}

bool RenderCore::GetmSampleCount() const
{
	return mSampleCount;
}

bool RenderCore::GetMsaaState() const
{
	return mMsaaState;
}

void RenderCore::SetMsaaState(bool value,HWND &WinHandle)
{
	if (mMsaaState != value)
	{
		mMsaaState = value;

		//重建交换链链和缓冲区
		CreateSwapChain(WinHandle);
		OnResize();
	}
}

bool RenderCore::Initialize()
{
	srand((UINT32)time(0));//用时间做随机数种子,保证程序执行的是"真随机数"
	return true;
}

void RenderCore::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}

void RenderCore::OnResize()
{
	assert(md3dDevice);
	assert(mSwapChain);
	assert(mDirectCmdListAlloc);

	// Flush before changing any resources.
	FlushCommandQueue();

	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// Release the previous resources we will be recreating.
	for (int32_t i = 0; i < SwapChainBufferCount; ++i)
		mSwapChainBuffer[i].Reset();
	mDepthStencilBuffer.Reset();

	// Resize the swap chain.
	ThrowIfFailed(mSwapChain->ResizeBuffers(
		SwapChainBufferCount,
		mClientWidth, mClientHeight,
		mBackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	mCurrBackBuffer = 0;

	CreateRTV();
	CreateDSV();

	//将资源从其初始状态转换为用作深度缓冲区
	mCommandList->ResourceBarrier(1,GetTypePointer(CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,			//转换前状态（创建时的状态，即CreateCommittedResource函数中定义的状态）
		D3D12_RESOURCE_STATE_DEPTH_WRITE)));	//转换后状态为可写入的深度图，还有一个D3D12_RESOURCE_STATE_DEPTH_READ是只可读的深度图)
	
	// Execute the resize commands.
	ThrowIfFailed(mCommandList->Close());								//命令添加完后将其关闭
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };			//声明并定义命令列表数组
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);	//将命令从命令列表传至命令队列

	//等到调整大小完成 
	FlushCommandQueue();
	//设置视口和裁剪矩形
	CreateViewPortAndScissorRect();
}

void RenderCore::CreateRTV()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		//获得存于交换链中的后台缓冲区资源
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
		md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(),
			nullptr,		//在交换链创建中已经定义了该资源的数据格式，所以这里指定为空指针
			rtvHeapHandle);	//描述符句柄结构体（这里是变体，继承自CD3DX12_CPU_DESCRIPTOR_HANDLE）
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);//偏移到描述符堆中的下一个缓冲区
	}
}

void RenderCore::CreateDSV()
{
	//在CPU中创建好深度模板数据资源
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; //指定资源维度（类型）为TEXTURE2D
	depthStencilDesc.Alignment = 0;									 //指定对齐
	depthStencilDesc.Width = mClientWidth;							 //资源宽
	depthStencilDesc.Height = mClientHeight;						 //资源高
	depthStencilDesc.DepthOrArraySize = 1;							 //纹理深度为1
	depthStencilDesc.MipLevels = 1;									 //MIPMAP层级数量
	depthStencilDesc.Format = mDepthStencilFormat;					 //24位深度，8位模板,还有个无类型的格式DXGI_FORMAT_R24G8_TYPELESS也可以使用
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;						 //多重采样质量
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;			 //指定纹理布局（这里不指定）
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//深度模板资源的Flag

	D3D12_CLEAR_VALUE optClear;				//清除资源的优化值，提高清除操作的执行速度（CreateCommittedResource函数中传入）
	optClear.Format = mDepthStencilFormat;	//24位深度，8位模板,还有个无类型的格式DXGI_FORMAT_R24G8_TYPELESS也可以使用
	optClear.DepthStencil.Depth = 1.0f;		//初始深度值为1
	optClear.DepthStencil.Stencil = 0;		//初始模板值为0

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		GetTypePointer(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)),
		D3D12_HEAP_FLAG_NONE,			                   //Flag
		&depthStencilDesc,				                   //上面定义的DSV资源指针
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,						                   //上面定义的优化值指针
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));//返回深度模板资源

	//创建DSV(必须填充DSV属性结构体，和创建RTV不同，RTV是通过句柄)
	//D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	//dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	//dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	//dsvDesc.Texture2D.MipSlice = 0;

	//创建描述符以使用资源的格式对整个资源进行 mip 级别 0
	md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(),
		nullptr,			//D3D12_DEPTH_STENCIL_VIEW_DESC类型指针，可填&dsvDesc（见上注释代码）
							//由于在创建深度模板资源时已经定义深度模板数据属性，所以这里可以指定为空指针
		DepthStencilView());//DSV句柄
}

void RenderCore::CreateViewPortAndScissorRect()
{
	//更新视口转换以覆盖客户区域
	mScreenViewport.TopLeftX = 0;//-hClientWidth/4//想要将视口对准缓冲区左半部分，即让图形绘制在缓冲区左半部分，我们只需更改TopLeftX，让绘制图形左移即可。
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mClientWidth);//hClientWidth/2//Width和Height是缓冲区的长宽，为了不产生图像拉伸缩放，这两个值和窗口的Width、Height比例是相等的。
	mScreenViewport.Height = static_cast<float>(mClientHeight);//hClientWidth/2
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	//裁剪矩形设置（矩形外的像素都将被剔除）
   //前两个为左上点坐标，后两个为右下点坐标
	mScissorRect = { 0, 0, mClientWidth, mClientHeight };//通过设置显示的范围
}

bool RenderCore::InitDirect3D(HWND& WinHandle)
{

//if use NV Nsight Debug,please don't set ID3D12Debug
//#if defined(DEBUG) || defined(_DEBUG)
//	{
//		//启用DX12调试层
//		ComPtr<ID3D12Debug> debugController;
//		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
//		debugController->EnableDebugLayer();
//	}
//#endif

	CreateDeviceAndFactory();//创建设备和工厂
	CreateFence();           //创建围栏
	GetDescriptorSize();     //获取描述符大小
	CheckMSAA();             //检查对MSAA技术的支持

	//#ifdef _DEBUG
	//	LogAdapters();
	//#endif

	CreateCommandObjects();
	CreateSwapChain(WinHandle);
	CreateRtvAndDsvDescriptorHeaps();

	return true;
}

void RenderCore::CreateDeviceAndFactory()
{
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));

	HRESULT hardwareResult = E_FAIL;

	//用于创建目标功能级别的设备
	auto tryCreateDevice = [this,&hardwareResult](D3D_FEATURE_LEVEL level,IUnknown* Adapter = nullptr){
		hardwareResult = D3D12CreateDevice(
			Adapter,
			level,
			IID_PPV_ARGS(md3dDevice.GetAddressOf()));
	};

	//尝试创建DX12U硬件设备
	tryCreateDevice(D3D_FEATURE_LEVEL_12_2);

	//失败,创建DX12_1设备
	if (FAILED(hardwareResult))
		tryCreateDevice(D3D_FEATURE_LEVEL_12_1);

	//失败,创建DX12_0设备
	if (FAILED(hardwareResult))
		tryCreateDevice(D3D_FEATURE_LEVEL_12_0);

	//无法创建D3d12设备,回退到WARP设备dx11_0
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
		tryCreateDevice(D3D_FEATURE_LEVEL_11_0, pWarpAdapter.Get());
	}

	//无法创建设备
	ThrowIfFailed(hardwareResult);
}

void RenderCore::CreateFence()
{
	ThrowIfFailed(md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(mFence.GetAddressOf())));
}

void RenderCore::GetDescriptorSize()
{
	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void RenderCore::CheckMSAA()
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.SampleCount = 4;

	for (auto SampleCount = 1; SampleCount <= D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT; SampleCount *= 2)
	{
		msQualityLevels.NumQualityLevels = 0;

		//当前图形驱动对MSAA多重采样的支持（注意：第二个参数即是输入又是输出）
		ThrowIfFailed(md3dDevice->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQualityLevels,
			sizeof(msQualityLevels)));

		if (msQualityLevels.NumQualityLevels <= 0)
			break;
		else
		{
			//如果支持MSAA，则Check函数返回的NumQualityLevels > 0
			mMsaaQuality = msQualityLevels.NumQualityLevels;
			mSampleCount = SampleCount;
		}
	}

	//expression为假（即为0），则终止程序运行，并打印一条出错信息
	assert(mMsaaQuality > 0 && "Unexpected MSAA quality level.");
}

void RenderCore::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(md3dDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&mCommandQueue)));

	ThrowIfFailed(md3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));

	ThrowIfFailed(md3dDevice->CreateCommandList(
		0,											//掩码值为0，单GPU
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCmdListAlloc.Get(),					//关联命令分配器接口指针
		nullptr,									//初始化流水线状态对象,这里不绘制
		IID_PPV_ARGS(mCommandList.GetAddressOf())));//返回创建的命令列表

	//首先要将命令列表至于关闭状态，第一次引用命令列表需要进行重置，调用重置前需要关闭
	mCommandList->Close();
}

void RenderCore::CreateSwapChain(HWND &WinHandle)
{
	//释放之前创建的交换链再重建
	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC swapChainDesc;						//交换链描述结构体
	swapChainDesc.BufferDesc.Width = mClientWidth;			//缓冲区分辨率的宽度
	swapChainDesc.BufferDesc.Height = mClientHeight;		//缓冲区分辨率的高度
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;	//刷新率的分母
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;	//刷新率的分子
	swapChainDesc.BufferDesc.Format = mBackBufferFormat;	//缓冲区的显示格式
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = SwapChainBufferCount;		//固定写法
	swapChainDesc.OutputWindow = WinHandle;					//渲染窗口句柄
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//使用命令队列对交换链刷新
	ThrowIfFailed(mdxgiFactory->CreateSwapChain(
		mCommandQueue.Get(),
		&swapChainDesc,
		(IDXGISwapChain**)mSwapChain.GetAddressOf()));
}

void RenderCore::FlushCommandQueue()
{
	//将围栏值向前移动以将命令标记到此围栏点
	//CPU传完命令并关闭后，将当前围栏值+1
	mCurrentFence++;

	// Add an instruction to the command queue to set a new fence point.  Because we
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	//当GPU处理完CPU传入的命令后，将fence接口中的围栏值+1，即fence->GetCompletedValue()+1
	ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));

	//等到 GPU 完成命令,直到此围栏点,如果小于，说明GPU没有处理完所有命令
	if (mFence->GetCompletedValue() < mCurrentFence)
	{
		//HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		HANDLE eventHandle = CreateEvent(nullptr, false, false, L"FenceSetDone");//创建事件

		//GPU 命中当前围栏时触发事件
		//当围栏达到mCurrentFence值（即执行到Signal（）指令修改了围栏值）时触发的eventHandle事件
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

		//等待 GPU 命中当前围栏事件被触发
		//阻塞当前线程直到事件触发，注意此Enent需先设置再等待，
		//如果没有Set就Wait，就死锁了，Set永远不会调用，所以也就没线程可以唤醒这个线程）
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

ID3D12Resource* RenderCore::CurrentBackBuffer() const
{
	return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderCore::CurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		mCurrBackBuffer,
		mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderCore::DepthStencilView() const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();	//DSV句柄
}


wstring RenderCore::CalculateFrameStats()
{
	static int_fast32_t frameCount = 0, fps;
	static float timeElapsed = 0.0f , msfi;

	frameCount++;

	//计算平均帧延迟和平均帧数
	if ((mTimer.TotalTime() - timeElapsed) >= mDetermineInterval)
	{
		fps = (int_fast32_t)(frameCount / mDetermineInterval);
		msfi = 1000.0f / fps;//毫秒单位的帧间隔

		// Reset for next average.
		frameCount = 0;
		timeElapsed += 1.0f;

		if (mShowFps)
		{
			wstring fpsStr = to_wstring(fps);
			wstring msfiStr = to_wstring(msfi);

			wstring windowText = mMainWndCaption +
				L"			 FPS: " + fpsStr +
				L",FrameInterval: " + msfiStr + L"(ms)";

			return windowText;
		}
	}
	return wstring(L"");
}

void RenderCore::SetDetermineInterval(float interval)
{
	if (interval < 0.20f || interval>2.0f)
		mDetermineInterval = 1.0f;
	else
		mDetermineInterval = interval;
}

void RenderCore::SetShowFps(bool flag)
{
	mShowFps = flag;
}

void RenderCore::LogAdapters()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (mdxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		adapterList.push_back(adapter);

		++i;
	}

	for (size_t i = 0; i < adapterList.size(); ++i)
	{
		LogAdapterOutputs(adapterList[i]);
		ReleaseCom(adapterList[i]);
	}
}

void RenderCore::LogAdapterOutputs(IDXGIAdapter* adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugString(text.c_str());

		LogOutputDisplayModes(output, mBackBufferFormat);

		ReleaseCom(output);

		++i;
	}
}

void RenderCore::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;

	// Call with nullptr to get list count.
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width\t= " + std::to_wstring(x.Width) + L" " +
			L"Height\t= " + std::to_wstring(x.Height) + L" " +
			L"Refresh\t= " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
			L"\n";

		::OutputDebugString(text.c_str());
	}
}
