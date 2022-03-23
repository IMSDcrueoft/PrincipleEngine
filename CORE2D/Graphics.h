#pragma once
#ifndef GRAPHICS
#define GRAPHICS

#include<Windows.h>
#include <wrl.h>
#include<cstdint>
#include<d2d1_3.h>
#include"D2DUIdef.h"
#pragma comment(lib,"d2d1.lib")

class Graphics
{
public:
	Graphics() = default;

	bool Init(
		_In_ const HWND& windowHandle);

	bool ResizeRenderTarget(
		_In_ const HWND& windowHandle);

	ID2D1RenderTarget* GetRenderTarget() const;

	void ClearScreenWithColor(
		_In_ const D2D1::ColorF& color = D2D1::ColorF{ 0xFF'FF'FF,1.0f });

	void ClearScreenWithTransparent();

	void BeginDraw();

	void EndDraw();

	void ClearRenderTargetTransform();

	void SetClip();

	void SetRenderTargetTransform(
		_In_ const FLOAT& angle,
		_In_ const PointF& center);

	void DrawCircle(
		_In_ const PointF& pos,
		_In_ const float& radius,
		_In_ const D2D1::ColorF& color,
		_In_ const float& thickness = 1.0f);

	void FillCircle(
		_In_ const PointF& pos,
		_In_ const float& radius,
		_In_ const D2D1::ColorF& color);

	void DrawEllipse(
		_In_ const PointF& pos,
		_In_ const RadiusXY& radius,
		_In_ const D2D1::ColorF& color,
		_In_ const float& thickness = 1.0f);

	void FillEllipse(
		_In_ const PointF& pos,
		_In_ const RadiusXY& radius,
		_In_ const D2D1::ColorF& color);

	//Draw Point is as same as Fill tiny circle

	void DrawLine(
		_In_ const PointF& start,
		_In_ const PointF& end,
		_In_ const D2D1::ColorF& color,
		_In_ const float& thickness = 1.0f);

	void DrawRectangle(
		_In_ const PointF& leftTop,
		_In_ const PointF& rightBottom,
		_In_ const D2D1::ColorF& color,
		_In_ const float& thickness = 1.0f);

	void FillRectangle(
		_In_ const PointF& leftTop,
		_In_ const PointF& rightBottom,
		_In_ const D2D1::ColorF& color);

	void DrawRoundRectangle(
		_In_ const PointF& leftTop,
		_In_ const PointF& rightBottom,
		_In_ const RadiusXY& radius,
		_In_ const D2D1::ColorF& color,
		_In_ const float& thickness = 1.0f);

	void FillRoundRectangle(
		_In_ const PointF& leftTop,
		_In_ const PointF& rightBottom,
		_In_ const RadiusXY& radius,
		_In_ const D2D1::ColorF& color);

private:
	Microsoft::WRL::ComPtr<ID2D1Factory> factory;
	Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> renderTarget;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> solidBrush;
};

#ifndef SafeRelease
#define SafeRelease(p)	\
{						\
	if (p != nullptr)	\
		p->Release();	\
	p = nullptr;		\
}
#endif
#endif // !GRAPHICS