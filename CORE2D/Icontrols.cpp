#include"Icontrols.h"
namespace D2DUI
{
	void ControlEvent::SetKeyboardEvent(void(*Event)())
	{
		this->KeyBoardEvent = Event;
	}

	void ControlEvent::SetMouseTouchEvent(void(*Event)())
	{
		this->MouseTouchEvent = Event;
	}

	void ControlEvent::SetMouseAwayEvent(void(*Event)())
	{
		this->MouseAwayEvent = Event;
	}

	void ControlEvent::SetMouseClickedEvent(void(*Event)())
	{
		this->MouseClickedEvent = Event;
	}

	void ControlEvent::SetMouseReleasedEvent(void(*Event)())
	{
		this->MouseReleasedEvent = Event;
	}

	void ControlContainer::AddControl(const std::wstring& name, Icontrols* control, const ControlType& type)
	{
		this->ChildComsMap[name] = ChildControl{ UICom_Ptr(control),type };
	}

	void ControlContainer::RemoveControl(const std::wstring& name)
	{
		this->ChildComsMap.erase(name);
	}

	ControlContainer::ChildControl& ControlContainer::GetControl(const std::wstring& name)
	{
		return this->ChildComsMap[name];
	}

	void BackgroundAndBorder::SetHasBorder()
	{
		this->hasBorder = true;
	}

	void BackgroundAndBorder::RemoveHasBorder()
	{
		this->hasBorder = false;
	}

	const bool& BackgroundAndBorder::GetHasBorder() const
	{
		return this->hasBorder;
	}

	void BackgroundAndBorder::SetHasRoundBorder(const RadiusXY& Rxy)
	{
		this->hasRoundBorder = true;
		this->borderRound = Rxy;
	}

	void BackgroundAndBorder::RemoveHasRoundBorder()
	{
		this->hasRoundBorder = false;
	}

	const RadiusXY& BackgroundAndBorder::GetRoundBorderRadiusXY() const
	{
		return this->borderRound;
	}

	void BackgroundAndBorder::SetBorderColor(const ColorF& color)
	{
		this->borderColor = color;
	}

	const ColorF& BackgroundAndBorder::GetBorderColor() const
	{
		return this->borderColor;
	}

	void BackgroundAndBorder::SetBackgroundColor(const ColorF& color)
	{
		this->backgroundColor = color;
	}

	const ColorF& BackgroundAndBorder::GetBackgroundColor() const
	{
		return this->backgroundColor;
	}

	void BackgroundImage::SetBoxSize(const TextureSize& drawSize)
	{
		this->BackgroundImageBoxSize = drawSize;
	}

	const TextureSize& BackgroundImage::GetImageSize() const
	{
		return this->image->GetSpriteSize();
	}

	const TextureSize& BackgroundImage::GetBoxSize() const
	{
		return this->BackgroundImageBoxSize;
	}

	void BackgroundImage::SetStyleConformal()
	{
		this->conformalOrAdapt = true;
	}

	void BackgroundImage::SetStyleAdapt()
	{
		this->conformalOrAdapt = false;
	}

	const bool& BackgroundImage::GetStyle() const
	{
		return this->conformalOrAdapt;
	}

	void ControlCommon::CalculateRealPosition(const PointU& containerSize)
	{
		this->realPosition = PointAdd(this->fixedPosition, PointMul(containerSize, this->relativePosition));
	}

}