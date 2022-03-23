#pragma once
#ifndef UICOM_MANAGER
#define UICOM_MANAGER

#include<forward_list>
#include"Icontrols.h"
#include"SpriteSheet.h"
#include"imageBox.h"

namespace D2DUI
{
	class UICom_Manager
	{
	public:
		static UICom_Manager* GetManager();

		template<typename Type_UICom>
		static Type_UICom* TryGetComponent(_In_ Icontrols* Icomponent);

	public:
		template<typename Type_UICom>
		void CreateComponent(_Out_ Type_UICom** component);

		void CreateSpriteFromFile(const std::wstring&,LPCWSTR, Graphics*);

		template<typename Type_UICom>
		void GetSpriteFromMap(const std::wstring&, Type_UICom*);
		void GetSpriteFromMap(const std::wstring&, BackgroundImage*);

		//must call in main/WinMain
		void ReleaseSpritesAndComs();

		UICom_Manager(const UICom_Manager&) = delete;
		UICom_Manager& operator=(const UICom_Manager&) = delete;


	private:
		UICom_Manager() = default;
		~UICom_Manager() = default;

	private:
		class Destructor
		{
		public:
			~Destructor();
		};
		static Destructor destructor;
		static UICom_Manager* Manager;

		std::unordered_map<std::wstring, std::shared_ptr<SpriteSheet>> spriteMap;
		std::forward_list<Icontrols*> uiComList;
	};

	inline UICom_Manager* UICom_Manager::Manager = new UICom_Manager();
	inline UICom_Manager::Destructor UICom_Manager::destructor = UICom_Manager::Destructor();

	//-----------------------------------------------------------------------------------------------------------
	inline UICom_Manager* UICom_Manager::GetManager()
	{
		return Manager;
	}

	inline void UICom_Manager::CreateSpriteFromFile(const std::wstring& spriteName,LPCWSTR fileName, Graphics* gfx)
	{
		auto sprite = std::make_shared<SpriteSheet>(fileName, gfx);

		if (sprite != nullptr)
		{
			spriteMap[spriteName] = sprite;
			sprite.reset();
		}
	}

	inline void UICom_Manager::ReleaseSpritesAndComs()
	{
		spriteMap.clear();

		//release coms in Manager
		for (auto& ui : uiComList)
		{
			delete ui;
		}
		uiComList.clear();
	}

	template<typename Type_UICom>
	inline Type_UICom* UICom_Manager::TryGetComponent(_In_ Icontrols* Icomponent)
	{
		if (Icomponent == nullptr)
			return nullptr;
		else
			return dynamic_cast<Type_UICom*>(Icomponent);
	}

	template<typename Type_UICom>
	inline void UICom_Manager::CreateComponent(_Out_ Type_UICom** component)
	{
		*component = nullptr;
		*component = new Type_UICom();
		Icontrols* Icom = dynamic_cast<Icontrols*>(*component);

		if (Icom != nullptr)
			uiComList.push_front(Icom);
		else
			ReleasePtr(*component);
	}

	template<typename Type_UICom>
	inline void UICom_Manager::GetSpriteFromMap(const std::wstring& spriteName, Type_UICom* com)
	{
		if (auto backImg = dynamic_cast<BackgroundImage*>(com); backImg != nullptr)
			GetSpriteFromMap(spriteName, backImg);
	}

	inline void UICom_Manager::GetSpriteFromMap(const std::wstring& spriteName, BackgroundImage* backImg)
	{
		backImg->image = spriteMap[spriteName];
	}

	inline UICom_Manager::Destructor::~Destructor()
	{
		ReleasePtr(UICom_Manager::Manager);
	}
}

#endif