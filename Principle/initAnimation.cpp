#include "initAnimation.h"

void initAnimation::createAnimateWin()
{
	WNDCLASS wcex;

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MsgProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = L"AnimeWnd";

	if (!RegisterClass(&wcex))
	{
		MessageBox(0, L"RegisterAnimeWndClass Failed!", 0, 0);
		return;
	}

	//计算系统矩阵
	RECT scrRect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &scrRect, 0);
	int32_t scrHeight_1_2 = (scrRect.bottom - scrRect.top) / 2;
	int32_t scrWidth = scrRect.right - scrRect.left;

	RECT targetRect = { 0, 0,scrHeight_1_2,  scrHeight_1_2 };
	AdjustWindowRect(&targetRect, WS_OVERLAPPEDWINDOW, false);

	hwnd = CreateWindow(L"AnimeWnd",L"Loading . . .",
		WS_POPUP &~WS_SIZEBOX & WS_CAPTION, CW_USEDEFAULT, CW_USEDEFAULT,
		targetRect.right - targetRect.left,
		targetRect.bottom - targetRect.top,
		0, 0, hInst, 0);

	if (!hInst)
	{
		MessageBox(0, L"CreateAnimeWnd Failed.", 0, 0);
		return;
	}

	MoveWindow(hwnd,  abs((scrWidth - scrHeight_1_2) / 2), scrHeight_1_2/2, scrHeight_1_2, scrHeight_1_2, 1);// 居中
	ShowWindow(hwnd, SW_SHOW);

	UpdateWindow(hwnd);
}

void initAnimation::Run()
{
	hInst = GetModuleHandle(0);

	createAnimateWin();

	MSG msg = { 0 };

	while (GetMessage(&msg, NULL, 0, 0)) {

		play();
		playTimes--;

		if ((playTimes > 0) || !endAnimeNow)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			SendMessage(hwnd,WM_CLOSE, 0, 0);
	}
}

initAnimation::initAnimation(uint32_t times) :playTimes(times)
{
	initThread = std::make_unique<std::thread>(&initAnimation::Run,this);
}

initAnimation::~initAnimation()
{
	initThread->~thread();
}

void initAnimation::play()
{
	//play pictures or vedios
}

void initAnimation::waitAnimationFinish()
{
	endAnimeNow = true;

	if (initThread->joinable())
		initThread->join();

	this->~initAnimation();
}

LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);

	case WM_NCLBUTTONDOWN://不允许移动窗口
	{
		switch (wParam)
		{
		case HTCAPTION:
			return 0;
		default:
			return 0;
		}
	}
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return(0);
}


