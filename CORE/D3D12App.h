#pragma once
#ifndef D3D12APP_H
#define D3D12APP_H

//头文件
#include "D3DMethods.h"
#include "RenderCore.h"
#include "PerformanceTimer.h"
#include "Interfaces.h"

// 框架类
class D3D12App:public RenderCore,public Win32MouseExtension,public Win32KeyStateExtension
{
protected:
	D3D12App(HINSTANCE hInstance, std::wstring wndCaption = L"");
	D3D12App(const D3D12App& rhs) = delete;
	D3D12App& operator=(const D3D12App& rhs) = delete;
	virtual ~D3D12App();

public:
	static D3D12App* GetApp();

	HINSTANCE AppInst()const;
	HWND      MainWnd()const;

	virtual INT Run() override;

	virtual bool Initialize() override;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:

	virtual void OnResize()override;

	bool InitMainWindow();		//创建WIN32主窗口

	//窗口的显示与隐藏
	void mHideWindow();//初始化时默认隐藏窗口
	void mShowWindow();//初始化完成请调用显示窗口----------------->务必调用

	void SetFrameStats();
protected:
	static D3D12App* mApp;

	HINSTANCE mhAppInst = nullptr; 	//实例句柄
	HWND      mhMainWnd = nullptr; 	//主窗口句柄
};

inline D3D12App* D3D12App::mApp = nullptr;
#endif // !D3D12APP