#include <cassert>
#include"SpriteSheet.h"

SpriteSheet::SpriteSheet(
	_In_ const std::wstring& filename,
	_In_ Graphics* gfx)
{
	bitmap = nullptr;

	sprite.x = 0;
	sprite.y = 0;

	spriteAccross.x = 1;
	spriteAccross.y = 1;

	graphics = gfx;

	HRESULT hr = S_FALSE;
	IWICImagingFactory * wicFactory = nullptr;

	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*)&wicFactory);

	IWICBitmapDecoder * wicDecoder = nullptr;
	hr = wicFactory->CreateDecoderFromFilename(
		filename.c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&wicDecoder);

	if (hr != S_OK)
		return;

	IWICBitmapFrameDecode * wicFrame = nullptr;
	hr = wicDecoder -> GetFrame(0, &wicFrame);


	IWICFormatConverter * wicCoverter = nullptr;
	hr = wicFactory->CreateFormatConverter(&wicCoverter);


	hr = wicCoverter->Initialize(
		wicFrame,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0,
		WICBitmapPaletteTypeCustom);

	graphics->GetRenderTarget()->CreateBitmapFromWicBitmap(
		wicCoverter,
		nullptr,
		bitmap.GetAddressOf());

	SafeRelease(wicFactory);
	SafeRelease(wicDecoder);
	SafeRelease(wicCoverter);
	SafeRelease(wicFrame);

	if(bitmap != nullptr)
	{
		sprite.x = (uint32_t)bitmap->GetSize().width;
		sprite.y = (uint32_t)bitmap->GetSize().height;
	}
}

SpriteSheet::SpriteSheet(
	_In_ const std::wstring& filename,
	_In_ Graphics* gfx,
	_In_ const TextureSize& size):
	SpriteSheet(filename,gfx)
{
	SetSpriteSize(size);
}

void SpriteSheet::SetGraphics(_In_ Graphics* gfx)
{
	if (this->graphics != gfx)
		this->graphics = gfx;
}

const TextureSize& SpriteSheet::GetSpriteSize()const
{
	return sprite;
}

void SpriteSheet::SetSpriteSize(
	_In_ const TextureSize& size)
{
	sprite = size;
	spriteAccross.x = (uint32_t)(bitmap->GetSize().width / size.x);
	spriteAccross.y = (uint32_t)(bitmap->GetSize().height / size.y);
}

const DivisionXY& SpriteSheet::GetDivisionXY() const
{
	return spriteAccross;
}

void SpriteSheet::SetDivisionXY(
	_In_ const DivisionXY& divXY)
{
	spriteAccross = divXY;
	sprite.x = (uint32_t)(bitmap->GetSize().width / divXY.x);
	sprite.y = (uint32_t)(bitmap->GetSize().height / divXY.y);
}

void SpriteSheet::Draw(
	_In_ const RectF& target,
	_In_ const RectF& source,
	_In_ const float& alpha,
	_In_ const D2D1_BITMAP_INTERPOLATION_MODE& interpolationMode)
{
	graphics->GetRenderTarget()->DrawBitmap(
		bitmap.Get(),
		target,
		std::clamp(alpha, 0.0f, 1.0f),
		interpolationMode,
		source);
}

RectF SpriteSheet::GetIndexSrcRect(_In_ const uint32_t& index)
{
	static PointL leftTop;
	leftTop.x = (index % spriteAccross.x) * sprite.x;
	leftTop.y = (index / spriteAccross.x) * sprite.y;

	return RectF
	{
		(float)leftTop.x,
		(float)leftTop.y,
		(float)(leftTop.x + sprite.x),
		(float)(leftTop.y + sprite.y)
	};
}

void SpriteSheet::Draw(
	_In_ const float& alpha,
	_In_ const D2D1_BITMAP_INTERPOLATION_MODE& interpolationMode)
{
	static RectF TexRect;

	TexRect = D2D1::RectF(
		0.0f,
		0.0f,
		bitmap->GetSize().width,
		bitmap->GetSize().height);

	Draw(TexRect, TexRect, alpha, interpolationMode);
}

void SpriteSheet::Draw(
	_In_ const PointL& dest,
	_In_ const float& zoom,
	_In_ const uint32_t& index,
	_In_ const float& alpha,
	_In_ const D2D1_BITMAP_INTERPOLATION_MODE& interpolationMode)
{
	static RectF destRect;

	destRect = D2D1::RectF(
		(float)dest.x,
		(float)dest.y,
		(float)(dest.x + sprite.x * zoom),
		(float)(dest.y + sprite.y * zoom));

	Draw(destRect, GetIndexSrcRect(index), alpha, interpolationMode);
}

void SpriteSheet::Draw(
	_In_ const PointL& dest,
	_In_ const TextureSize& targetSize,
	_In_ const uint32_t& index,
	_In_ const float& alpha,
	_In_ const D2D1_BITMAP_INTERPOLATION_MODE& interpolationMode)
{
	static RectF destRect;

	destRect = D2D1::RectF(
		(float)dest.x,
		(float)dest.y,
		(float)(dest.x + targetSize.x),
		(float)(dest.y + targetSize.y));

	Draw(destRect, GetIndexSrcRect(index), alpha, interpolationMode);
}
