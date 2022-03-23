#include"Graphics.h"

bool Graphics::Init(_In_ const HWND& windowHandle)
{
	HRESULT res = S_FALSE;
	//create factory
	res = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, factory.GetAddressOf());

	if (res != S_OK)
		return false;

	RECT rect;
	GetClientRect(windowHandle, &rect);

	//create renderTarget
	factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			windowHandle, 
			D2D1::SizeU(rect.right, rect.bottom)),
		&renderTarget);

	if (res != S_OK)
		return false;

	//create SolidColorBrush
	renderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &solidBrush);

	if (res != S_OK)
		return false;

	//all fine
	return true;
}

ID2D1RenderTarget* Graphics::GetRenderTarget()const
{
	return renderTarget.Get();
}

void Graphics::BeginDraw()
{
	renderTarget->BeginDraw();
}

void Graphics::EndDraw()
{
	renderTarget->EndDraw();
}

bool Graphics::ResizeRenderTarget(_In_ const HWND& windowHandle)
{
	RECT rect;
	GetClientRect(windowHandle, &rect);
	
	if (S_OK == renderTarget->Resize(D2D1::SizeU(rect.right, rect.bottom)))
		return false;

	return true;
}

void Graphics::ClearRenderTargetTransform()
{
	renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}

void Graphics::SetRenderTargetTransform(
	_In_ const FLOAT& angle,
	_In_ const PointF& center)
{
	renderTarget->SetTransform(
		D2D1::Matrix3x2F::Rotation(
			angle,
			center));
}

void Graphics::ClearScreenWithColor(_In_ const D2D1::ColorF& color)
{
	renderTarget->Clear(color);
}

void Graphics::ClearScreenWithTransparent()
{
	renderTarget->Clear();
}

void Graphics::DrawCircle(
	_In_ const PointF& pos,
	_In_ const float& radius,
	_In_ const D2D1::ColorF& color,
	_In_ const float& thickness)
{
	PointF radiusXY{ radius,radius };
	DrawEllipse(pos, radiusXY, color, thickness);
}

void Graphics::FillCircle(
	_In_ const PointF& pos,
	_In_ const float& radius,
	_In_ const D2D1::ColorF& color)
{
	PointF radiusXY{ radius,radius };
	FillEllipse(pos, radiusXY, color);
}

void Graphics::DrawEllipse(
	_In_ const PointF& pos,
	_In_ const RadiusXY& radius,
	_In_ const D2D1::ColorF& color,
	_In_ const float& thickness)
{
	solidBrush->SetColor(color);
	renderTarget->DrawEllipse(D2D1::Ellipse(pos, radius.x, radius.y), solidBrush.Get(), thickness);
}

void Graphics::FillEllipse(
	_In_ const PointF& pos,
	_In_ const RadiusXY& radius,
	_In_ const D2D1::ColorF& color)
{
	solidBrush->SetColor(color);
	renderTarget->FillEllipse(D2D1::Ellipse(pos, radius.x, radius.y), solidBrush.Get());
}

void Graphics::DrawLine(
	_In_ const PointF& start,
	_In_ const PointF& end,
	_In_ const D2D1::ColorF& color,
	_In_ const float& thickness)
{
	solidBrush->SetColor(color);
	renderTarget->DrawLine(start, end, solidBrush.Get(), thickness);
}

void Graphics::DrawRectangle(
	_In_ const PointF& leftTop,
	_In_ const PointF& rightBottom,
	_In_ const D2D1::ColorF& color,
	_In_ const float& thickness)
{
	solidBrush->SetColor(color);
	renderTarget->DrawRectangle(D2D1::RectF(leftTop.x, leftTop.y, rightBottom.x, rightBottom.y), solidBrush.Get(), thickness);
}

void Graphics::FillRectangle(
	_In_ const PointF& leftTop,
	_In_ const PointF& rightBottom,
	_In_ const D2D1::ColorF& color)
{
	solidBrush->SetColor(color);
	renderTarget->FillRectangle(D2D1::RectF(leftTop.x, leftTop.y, rightBottom.x, rightBottom.y), solidBrush.Get());
}

void Graphics::DrawRoundRectangle(
	_In_ const PointF& leftTop,
	_In_ const PointF& rightBottom,
	_In_ const RadiusXY& radius,
	_In_ const D2D1::ColorF& color,
	_In_ const float& thickness)
{
	solidBrush->SetColor(color);
	renderTarget->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(leftTop.x, leftTop.y, rightBottom.x, rightBottom.y), radius.x, radius.y), solidBrush.Get(), thickness);
}

void Graphics::FillRoundRectangle(
	_In_ const PointF& leftTop,
	_In_ const PointF& rightBottom,
	_In_ const RadiusXY& radius,
	_In_ const D2D1::ColorF& color)
{
	solidBrush->SetColor(color);
	renderTarget->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(leftTop.x, leftTop.y, rightBottom.x, rightBottom.y), radius.x, radius.y), solidBrush.Get());
}


