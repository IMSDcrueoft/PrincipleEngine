#pragma once
#ifndef RENDERCORE_H
#define RENDERCORE_H

// 链接必要库
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment (lib,"user32.lib" ) 

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// 头文件
#include "D3DMethods.h"
#include "PerformanceTimer.h"

// 渲染核心
class RenderCore
{
public:
	float AspectRatio()const;						 // 后台缓冲区宽高比

	bool GetmSampleCount()const;	                 // 获取最大采样数
	bool GetMsaaState	()const;					 // 功能是否已启用
	void SetMsaaState(bool value, HWND& WinHandle);  // 启动/禁用SMAA功能
	void SetShowFps  (bool flag = false);			 // 开关帧数延迟显示
	// 注意：DX12不支持交换链打开SMAA

	virtual INT Run() = 0;

	virtual bool Initialize();
protected:
	virtual void CreateRtvAndDsvDescriptorHeaps();
	virtual void OnResize ();					        // 如果窗口尺寸发生改变就需要调整某些属性以适配
	virtual void CreateRTV();					        // 先从RTV堆中拿到首个RTV句柄，然后获得存于交换链中的RT资源，最后创建RTV将RT资源和RTV句柄联系起来，并在最后根据RTV大小做了在堆中的地址偏移。
	virtual void CreateDSV();					        // 先在CPU中创建好DS资源，然后通过CreateCommittedResource函数将DS资源提交至GPU显存中，最后创建DSV将显存中的DS资源和DSV句柄联系起来。
	virtual void CreateViewPortAndScissorRect();        // 设置视口和裁剪矩形

	virtual void Update(const PerformanceTimer& pt) = 0;// 逻辑行为
	virtual void Draw(const PerformanceTimer& pt) = 0;	// 渲染动作

protected:
	bool InitDirect3D(HWND& WinHandle);	    // 初始化DirectX3D
	void CreateDeviceAndFactory();		    // 创建D3D设备
	void CreateFence();					    // 通过设备创建围栏fence,以便同步CPU和GPU
	void GetDescriptorSize();			    // 获取描述符大小-->RTV（渲染目标缓冲区）、DSV（深度模板缓冲区）、CBV_SRV_UAV（常量缓冲区、着色器资源缓冲和随机访问缓冲）
	void CheckMSAA();					    // 检查多重采样的支持

	void CreateCommandObjects();			// 创建命令队列列表及分配器
	void CreateSwapChain(HWND& WinHandle);	// 创建交换链

	void FlushCommandQueue();	            // 强制CPU等待GPU

	// 获取缓冲区RSV DSV
	ID3D12Resource* CurrentBackBuffer()const;					        // 返回交换链中当前后台缓冲区的 ID3D12Resource
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;	        // 返回RTV渲染目标
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;		        // 返回SDV渲染深度

	std::wstring CalculateFrameStats();					                // 计算每秒的平均帧数以及渲染延迟
	void SetDetermineInterval(float interval = 1.0f);		            // 设置新的测定间隔

	// 显示适配器相关
	void LogAdapters();													// 枚举显示适配器
	void LogAdapterOutputs(IDXGIAdapter* adapter);						// 枚举指定显示适配器的全部显示输出
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);// 枚举某个显示输出对特定格式支持的所有显示模式
protected:
	bool      mAppPaused       = false;  	   // 是否暂停中
	bool      mMinimized       = false;        // 是否最小化
	bool      mMaximized       = false;  	   // 是否最大化
	bool      mResizing        = false;        // 调整大小被拖拽中？
	bool      mFullscreenState = false;        // 全屏是否开启
	bool	  mShowFps         = true;         // 是否要持续显示FPS

	//设置true来应用4X MSAA (?.1.8).  默认false
	bool      mMsaaState = false;      // MSAA enabled
	UINT      mMsaaQuality = 0;        // 采样质量,useless
	UINT	  mSampleCount = 1;		   // 采样次数支持

protected:
	//性能计数器
	PerformanceTimer mTimer;

	//Microsoft::WRL::ComPtr<IDXGIFactory7> mdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGIFactory7> mdxgiFactory;
	//Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;
	//Microsoft::WRL::ComPtr<ID3D12Device5> md3dDevice;
	Microsoft::WRL::ComPtr<ID3D12Device5> md3dDevice;

	Microsoft::WRL::ComPtr<ID3D12Fence1> mFence;
	UINT64 mCurrentFence = 0;

	//命令队列和命令列表
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;			// 命令队列
	//Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> mCommandList;	// 命令列表
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> mCommandList;	// 命令列表
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;	// 命令分配器

	static constexpr INT SwapChainBufferCount = 2;		// 缓冲区尺寸
	INT mCurrBackBuffer = 0;							// 缓冲区索引

	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;// 渲染目标视图堆
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;// 深度模板视图堆

	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;

	// Derived class should set these in derived constructor to customize starting values.
	std::wstring mMainWndCaption = L"DirectX 3D App";

	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;// 归一化的无符号整型
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	INT mClientWidth = 1280;
	INT mClientHeight = 720;

	float mDetermineInterval = 1.0f;// 对于平均帧数和渲染延迟的测定间隔
};

#endif