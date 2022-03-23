#include"D3D12App.h"
#include <WindowsX.h>

//使用智能指针ComPtr
using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return D3D12App::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

D3D12App::D3D12App(HINSTANCE hInstance, std::wstring wndCaption)
	:mhAppInst(hInstance)
{
	if (wndCaption != L"")
		mMainWndCaption = wndCaption;

	//只允许一个APP
	assert(mApp == nullptr);
	mApp = this;
}

D3D12App::~D3D12App()
{
	//等待GPU完成队列任务，否则可能造成程序崩溃
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

D3D12App * D3D12App::GetApp()
{
	return mApp;
}

HINSTANCE D3D12App::AppInst() const
{
	return mhAppInst;
}

HWND D3D12App::MainWnd() const
{
	return mhMainWnd;
}

int32_t D3D12App::Run()
{
	MSG msg = { 0 };

	//重置计数器
	mTimer.Reset();

	while (msg.message != WM_QUIT)//消息循环
	{
		//处理消息
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else//执行逻辑
		{
			mTimer.Tick();

			if (!mAppPaused)//持续渲染
			{
				SetFrameStats();
				Update(mTimer);
				Draw(mTimer);
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (INT)msg.wParam;
}

bool D3D12App::Initialize()
{
	RenderCore::Initialize();

	if (!InitMainWindow())
		return false;

	if (!InitDirect3D(mhMainWnd))
		return false;

	OnResize();
	return true;
}

LRESULT D3D12App::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATE:
		//程序进入非活动状态
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.Start();
		}
		return 0;

		//用户正抓取调整栏
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return 0;

		//释放调整栏
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		OnResize();
		return 0;

	case WM_SIZE:
		// Save the new client area dimensions.
		mClientWidth = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);
		if (md3dDevice)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				// Restoring from minimized state?
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if (mResizing)
				{
					// If user is dragging the resize bars, we do not resize
					// the buffers here because as the user continuously
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is
					// done resizing the window and releases the resize bars, which
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		//防止窗口变得过小
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 400;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 300;
		return 0;

		//处理鼠标事件
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		else if ((int)wParam == VK_F2)
		{
			;//Set4xMsaaState(!m4xMsaaState);
		}
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void D3D12App::OnResize()
{
	RenderCore::OnResize();
	limitMouseMovementArea(mhMainWnd);
}

bool D3D12App::InitMainWindow()
{
	WNDCLASS wcex;

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = mhAppInst;
	wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wcex))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	//计算窗口矩形
	RECT WinRect = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&WinRect, WS_OVERLAPPEDWINDOW, false);
	INT width = WinRect.right - WinRect.left;
	INT height = WinRect.bottom - WinRect.top;

	mhMainWnd = CreateWindow(L"MainWnd", mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);

	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_HIDE);//直接隐藏窗口

	RECT scrRect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &scrRect, 0);
	MoveWindow(mhMainWnd, (scrRect.right - scrRect.left - width) / 2, (scrRect.bottom - scrRect.top - height) / 2, width, height, 1);

	UpdateWindow(mhMainWnd);

	return true;
}

void D3D12App::mHideWindow()
{
	if (IsWindowVisible(mhMainWnd))
		ShowWindow(mhMainWnd, SW_HIDE);
	//SendMessage(mhMainWnd, WM_CLOSE, 0, 0);
}

void D3D12App::mShowWindow()
{
	if (!IsWindowVisible(mhMainWnd))
		ShowWindow(mhMainWnd, SW_SHOW);
	//SendMessage(mhMainWnd, WM_SHOWWINDOW, 0, 0);
}

void D3D12App::SetFrameStats()
{
	if (auto&& wstr = RenderCore::CalculateFrameStats(); mShowFps && !wstr.empty())
		SetWindowText(mhMainWnd, wstr.c_str());
}