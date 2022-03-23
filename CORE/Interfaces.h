#pragma once
#ifndef INTERFACES
#define INTERFACES

#include<Windows.h>
#include<cstdint>
#include<memory>
#include"PerformanceTimer.h"
#include"../CORE2D/Graphics.h"
#include"../CORE2D/SpriteSheet.h"


/* use this to force cursor in window */
#define ForceCursorInClientWindow false


class Win32KeyStateExtension
{
public:
	/* vkeyCode:such as 'A','VK_LBUTTON' */
	static inline bool IsKeyDown(const int32_t& vkeyCode)
	{
		return ((GetAsyncKeyState(vkeyCode) & 0x8000) != (SHORT)0);
	};

	static inline bool IsKeyUp(const int32_t& vkeyCode)
	{
		return ((GetAsyncKeyState(vkeyCode) & 0x8000) == (SHORT)0);
	};

public:
	virtual void OnKeyboardInput(const PerformanceTimer&) {}
};



class Win32MouseExtension
{
public:
	/* show and hide cursor */
	inline void mShowCursor()
	{
		ShowCursor(true);
	};

	inline void mHideCursor()
	{
		ShowCursor(false);
	};

	inline void SetCursorToCenter()
	{
		SetCursorPos(mScreenCenterPos.x, mScreenCenterPos.y);
	}

protected:
	virtual void limitMouseMovementArea(const HWND& mhMainWnd);

	// 鼠标输入处理
	virtual void OnMouseDown(WPARAM btnState, int32_t x, int32_t y) { }
	virtual void OnMouseUp(WPARAM btnState, int32_t x, int32_t y) { }
	virtual void OnMouseMove(WPARAM btnState, int32_t x, int32_t y) { }

protected:

	POINT mLastMousePos;               // 用户区域的正中
	POINT mScreenCenterPos;            // 用户区域的正中对应屏幕区域的位置
	bool  mForceMouseToCenter = false; // 强制固定鼠标在正中
};
#endif // !INTERFACES