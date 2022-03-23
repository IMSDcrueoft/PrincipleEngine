#pragma once
#ifndef IMAGEBOX
#define IMAGEBOX
#include"Icontrols.h"
#include<memory>
namespace D2DUI
{
	class ImageBox :public Icontrols,public BackgroundImage,public ControlCommon
	{
	protected:
		friend class UICom_Manager;
		~ImageBox() = default;

	public:
		ImageBox();

		virtual void Init(Graphics* = nullptr)override;
		virtual void CheckMouse(const PointL&) override;
		virtual void Draw(Graphics*,const PointU&) override;

	public:
		virtual void Show() override;
		virtual void Hide() override;
		virtual const bool& getVisible() const override;

		virtual void SetFixedPosition(const PointL&) override;
		virtual const PointL& GetFixedPosition() const override;
		virtual void SetRelativePosition(const PointF&) override;
		virtual const PointF& GetRelativePosition() const override;

	public:
		virtual void OnKeyBoardInput() override;

		virtual void OnMouseTouch() override;
		virtual void OnMouseAway() override;
		virtual void OnMouseClicked() override;
		virtual void OnMouseReleased() override;
	};
}
#endif //IMAGEBOX