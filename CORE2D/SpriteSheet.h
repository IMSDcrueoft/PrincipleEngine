#pragma once
#ifndef SPRITESHEET
#define SPRITESHEET

#include<wincodec.h>
#include<cstdint>
#include<memory>
#include<algorithm>
#include"Graphics.h"
#include"D2DUIdef.h"
#include <string>

#pragma comment(lib,"windowscodecs.lib")

class SpriteSheet
{
public:
	SpriteSheet() = delete;
	~SpriteSheet() = default;

	SpriteSheet(
		_In_ const std::wstring& filename,
		_In_ Graphics* gfx);

	SpriteSheet(
		_In_ const std::wstring& filename,
		_In_ Graphics* gfx,
		_In_ const TextureSize& size);

	void SetGraphics(
		_In_ Graphics* gfx);

	void SetSpriteSize(
		_In_ const TextureSize& size);

	void SetDivisionXY(
		_In_ const DivisionXY& divXY);

	const TextureSize& GetSpriteSize()const;

	const DivisionXY& GetDivisionXY()const;

	//D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR 最近邻线性插值法
	//full BackgroundImageBoxSize,from (0,0)
	// 直接绘制
	void Draw(
		_In_ const float& alpha = 1.0,
		_In_ const D2D1_BITMAP_INTERPOLATION_MODE& interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

	// 按比例缩放
	void Draw(
		_In_ const PointL& dest,
		_In_ const float& zoom = 1.0f,
		_In_ const uint32_t& index = 0,
		_In_ const float& alpha = 1.0f,
		_In_ const D2D1_BITMAP_INTERPOLATION_MODE& interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

	// 按目标大小拉伸
	void Draw(
		_In_ const PointL& dest,
		_In_ const TextureSize& targetSize,
		_In_ const uint32_t& index = 0,
		_In_ const float& alpha = 1.0f,
		_In_ const D2D1_BITMAP_INTERPOLATION_MODE& interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

	// 原始绘制
	void Draw(
		_In_ const RectF& target,
		_In_ const RectF& source,
		_In_ const float& alpha,
		_In_ const D2D1_BITMAP_INTERPOLATION_MODE& interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

private:
		RectF GetIndexSrcRect(
		_In_  const uint32_t& index);

private:
	Microsoft::WRL::ComPtr<ID2D1Bitmap> bitmap;
	Graphics* graphics = nullptr;// don't delete

	TextureSize sprite;
	DivisionXY spriteAccross;
};
#endif // !SPRITESHEET
