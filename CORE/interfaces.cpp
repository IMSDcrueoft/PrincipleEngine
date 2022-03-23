#include"Interfaces.h"

void Win32MouseExtension::limitMouseMovementArea(const HWND& mhMainWnd)
{
	//限制鼠标光标移动区域
	static POINT leftTop, rightBottom;
	static RECT cursorRect;

	GetClientRect(mhMainWnd, &cursorRect);                 //取得窗口内部矩形

	leftTop = { cursorRect.left,cursorRect.top };          //将矩形左上点坐标存入

	rightBottom = { cursorRect.right,cursorRect.bottom };  //将矩形右下坐标存入

	mScreenCenterPos = { (rightBottom.x - leftTop.x) / 2,(rightBottom.y - leftTop.y) / 2 };//获取用户区的正中

	if (mForceMouseToCenter)
		mLastMousePos = mScreenCenterPos;		 // 如果强制鼠标在正中

	ClientToScreen(mhMainWnd, &mScreenCenterPos);//将窗口坐标转换为屏幕坐标

#if ForceCursorInClientWindow //如果限制鼠标在窗口中

	ClientToScreen(mhMainWnd, &leftTop);
	ClientToScreen(mhMainWnd, &rightBottom);

	cursorRect = { leftTop.x,leftTop.y,rightBottom.x,rightBottom.y };//以屏幕坐标重新设定矩形区域

	ClipCursor(&cursorRect);//限制鼠标光标移动区域

#endif
}