#pragma once

#ifndef CONTROLS
#define CONTROLS

#include <unordered_map>
#include "Graphics.h"
#include "SpriteSheet.h"
#include "UICom_Ptr.h"
#include "D2DUIdef.h"

namespace D2DUI
{
	class IcontrolEvent
	{
	public:
		virtual void OnKeyBoardInput() = 0;

		virtual void OnMouseTouch() = 0;
		virtual void OnMouseAway() = 0;
		virtual void OnMouseClicked() = 0;
		virtual void OnMouseReleased() = 0;
	};


	class IcontrolCommon
	{
	public:
		virtual void Show() = 0;
		virtual void Hide() = 0;
		virtual const bool& getVisible() const = 0;

		virtual void SetFixedPosition(const PointL&) = 0;
		virtual const PointL& GetFixedPosition() const = 0;
		virtual void SetRelativePosition(const PointF&) = 0;
		virtual const PointF& GetRelativePosition() const = 0;
	};


	/* Ui */
	class Icontrols:public IcontrolEvent,public IcontrolCommon
	{
	protected:
		friend class UICom_Manager;
		virtual ~Icontrols(){}

	public:
		virtual void Init(Graphics*) = 0;

		/* call it in the Response of the message loop */
		virtual void CheckMouse(const PointL&) = 0;

		virtual void Draw(Graphics*,const PointU&) = 0;
	};


	class ControlEvent
	{
	public:
		void SetKeyboardEvent(void (*Event)() = nullptr);

		void SetMouseTouchEvent(void (*Event)() = nullptr);
		void SetMouseAwayEvent(void (*Event)() = nullptr);
		void SetMouseClickedEvent(void (*Event)() = nullptr);
		void SetMouseReleasedEvent(void (*Event)() = nullptr);

	protected:
		void (*KeyBoardEvent)();

		void (*MouseTouchEvent)();
		void (*MouseAwayEvent)();
		void (*MouseClickedEvent)();
		void (*MouseReleasedEvent)();
	};

	
	class ControlContainer
	{
		enum class ControlType
		{
			Unknown,
			ImageBox,
			Label,
			Panel,
			TextBox,
			Button
		};

		struct ChildControl
		{
			UICom_Ptr<Icontrols> uiPtr;
			ControlType uiType;
		};

	public:
		void AddControl(const std::wstring& name, Icontrols* control, const ControlType& type);
		void RemoveControl(const std::wstring& name);
		ChildControl& GetControl(const std::wstring& name);

	protected:
		std::unordered_map<std::wstring, ChildControl> ChildComsMap;
	};


	class BackgroundAndBorder
	{
	public:
		void SetHasBorder();
		void RemoveHasBorder();
		const bool& GetHasBorder()const;

		void SetHasRoundBorder(const RadiusXY&);
		void RemoveHasRoundBorder();
		const RadiusXY& GetRoundBorderRadiusXY()const;

		void SetBorderColor(const ColorF&);
		const ColorF& GetBorderColor() const;

		void SetBackgroundColor(const ColorF&);
		const ColorF& GetBackgroundColor() const;

	protected:
		bool hasBorder;
		bool hasRoundBorder;

		RadiusXY borderRound;
		ColorF borderColor;
		ColorF backgroundColor;
	};


	class BackgroundImage
	{
		friend class UICom_Manager;
	public:
		void SetBoxSize(const TextureSize&);
		const TextureSize& GetImageSize()const;
		const TextureSize& GetBoxSize() const;

		void SetStyleConformal();
		void SetStyleAdapt();
		const bool& GetStyle()const;

	protected:
		bool conformalOrAdapt;

		TextureSize BackgroundImageBoxSize;
		std::shared_ptr<SpriteSheet> image;
	};


	class ControlCommon
	{
	public:
		void CalculateRealPosition(const PointU&);

	protected:
		bool visible;
		
		PointL fixedPosition;
		PointF relativePosition;

		PointL realPosition;
	};


#ifndef ReleasePtr
#define ReleasePtr(p)	\
{						\
	if (p != nullptr)	\
	{                   \
		delete p;		\
		p = nullptr;	\
	}                   \
}

}
#endif
#endif // !CONTROLS