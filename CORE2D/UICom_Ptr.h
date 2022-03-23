#pragma once
#ifndef UICOM_PTR
#define UICOM_PTR

#include <cstddef>
namespace D2DUI
{
	template<typename T>
	class UICom_Ptr
	{
		using Type_UICom = T;
	public:
		explicit UICom_Ptr(Type_UICom* = nullptr);
		~UICom_Ptr();

		bool operator==(const nullptr_t) const;
		bool operator!=(const nullptr_t) const;
		bool operator==(const UICom_Ptr<Type_UICom>&) const;
		bool operator!=(const UICom_Ptr<Type_UICom>&) const;

		Type_UICom& operator=(Type_UICom*);
		Type_UICom& operator*() const;

		Type_UICom* operator->() const;
		Type_UICom* Get() const;
		Type_UICom* Release();

		Type_UICom** GetAddressOf();
		Type_UICom** ReleaseAndGetAddressOf();

	private:
		Type_UICom* IPtr = nullptr;
	};

	//-----------------------------------------------------------------------------------------------------------

	template<typename Type_UICom>
	inline UICom_Ptr<Type_UICom>::UICom_Ptr(Type_UICom* ptr) :IPtr(ptr) {}

	template<typename T>
	inline UICom_Ptr<T>::~UICom_Ptr()
	{
		IPtr = nullptr;
	}

	template<typename Type_UICom>
	inline bool UICom_Ptr<Type_UICom>::operator==(const nullptr_t ptr) const
	{
		return (this->IPtr == ptr);
	}

	template<typename T>
	inline bool UICom_Ptr<T>::operator!=(const nullptr_t ptr) const
	{
		return (this->IPtr != ptr);
	}

	template<typename Type_UICom>
	inline bool UICom_Ptr<Type_UICom>::operator==(const UICom_Ptr<Type_UICom>& CPtr) const
	{
		return (this->IPtr == CPtr.IPtr);
	}

	template<typename Type_UICom>
	inline bool UICom_Ptr<Type_UICom>::operator!=(const UICom_Ptr<Type_UICom>& CPtr) const
	{
		return (this->IPtr != CPtr.IPtr);
	}

	template<typename Type_UICom>
	inline Type_UICom& UICom_Ptr<Type_UICom>::operator=(Type_UICom* ptr)
	{
		if (this->IPtr != ptr)
		{
			this->Release();
			this->IPtr = ptr;
		}

		return *this;
	}

	template<typename Type_UICom>
	inline Type_UICom& UICom_Ptr<Type_UICom>::operator*() const
	{
		return *IPtr;
	}

	template<typename Type_UICom>
	inline Type_UICom* UICom_Ptr<Type_UICom>::operator->() const
	{
		return IPtr;
	}

	template<typename Type_UICom>
	inline Type_UICom* UICom_Ptr<Type_UICom>::Get() const
	{
		return IPtr;
	}

	template<typename Type_UICom>
	inline Type_UICom* UICom_Ptr<Type_UICom>::Release()
	{
		Type_UICom* tempPtr = IPtr;
		IPtr = nullptr;
		return tempPtr;
	}

	template<typename Type_UICom>
	inline Type_UICom** UICom_Ptr<Type_UICom>::GetAddressOf()
	{
		return &IPtr;
	}

	template<typename Type_UICom>
	inline Type_UICom** UICom_Ptr<Type_UICom>::ReleaseAndGetAddressOf()
	{
		Type_UICom** tempPtr = &IPtr;
		IPtr = nullptr;
		return tempPtr;
	}
}
#endif // !UICOM_PTR
