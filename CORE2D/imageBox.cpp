#include"imageBox.h"

namespace D2DUI {
	ImageBox::ImageBox()
	{
		Init(nullptr);
	}

	void ImageBox::Init(Graphics*)
	{
		visible = true;
		fixedPosition = { 0,0 };
		relativePosition = { 0.0f,0.0f };
	}

	void D2DUI::ImageBox::Show()
	{
		visible = true;
	}

	void D2DUI::ImageBox::Hide()
	{
		visible = false;
	}

	const bool& D2DUI::ImageBox::getVisible() const
	{
		return visible;
	}

	void ImageBox::SetFixedPosition(const PointL& newPos)
	{
		this->fixedPosition = newPos;
	}

	const PointL& D2DUI::ImageBox::GetFixedPosition() const
	{
		return fixedPosition;
	}

	void ImageBox::SetRelativePosition(const PointF& newPos)
	{
		this->relativePosition = newPos;
	}

	const PointF& ImageBox::GetRelativePosition() const
	{
		return relativePosition;
	}

	void ImageBox::CheckMouse(const PointL&)
	{
	}

	void ImageBox::Draw(Graphics* gfx,const PointU& containerSize)
	{
		if (visible)
		{
			CalculateRealPosition(containerSize);

			if (image != nullptr)
			{
				this->image->SetGraphics(gfx);

				if (conformalOrAdapt)
				{
					PointF scale = PointDiv(PointTrans<float,PointF>(BackgroundImageBoxSize), PointTrans<float,PointF>(this->GetImageSize()));
					float s = (scale.x < scale.y) ? scale.x : scale.y;
					scale = { s,s };
					TextureSize newSize = PointMul(this->GetImageSize(), scale);
					this->image->Draw(this->realPosition, newSize);
				}
				else
				{
					this->image->Draw(this->realPosition, BackgroundImageBoxSize);
				}

				gfx->DrawRoundRectangle(
					PointTrans<float, PointF>(this->realPosition),
					PointAdd(PointTrans<float, PointF>(this->realPosition), PointTrans<float, PointF>(BackgroundImageBoxSize)),
					{ 4,4 },
					D2D1::ColorF{ 0xE0'E0'E0 });
			}
		}
	}

	void ImageBox::OnKeyBoardInput()
	{
	}

	void ImageBox::OnMouseTouch()
	{
	}

	void ImageBox::OnMouseAway()
	{
	}

	void ImageBox::OnMouseClicked()
	{
	}

	void ImageBox::OnMouseReleased()
	{
	}
}