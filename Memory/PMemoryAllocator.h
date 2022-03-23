#pragma once
#ifndef PMEMORYALLOCATOR
#define PMEMORYALLOCATOR

#include <climits>
#include "PMemoryPool.h"

/* 外部对象分配接口 */
template<typename T>
class Iallocator
{
public:
	virtual T* allocate(size_type size, const_unknown_pointer ptr = nullptr) = 0;
	virtual void deallocate(T* ptr, size_type size) = 0;
	virtual void construct(T* ptr, const T& value) = 0;
	virtual void destroy(T* ptr) = 0;
	virtual T* address(T& object) = 0;
	virtual const T* const_address(const T& object) = 0;
	virtual size_type max_size() const = 0;
};

/*
	外部对象分配器:调用
	注意:多线程调用不安全
	//std::pmr::unsynchronized_pool_resource 这俩区别是前者可以多线程使用(库作者负责保证多线程安全)
	//std::pmr::synchronized_pool_resource 后者在多线程下需要你自己保证多线程安全
*/
template<typename T>
class PMemoryPool :public Iallocator<T>
{
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;

	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using unknown_pointer = void*;
	using const_unknown_pointer = const void*;

	template <class U>
	struct rebind {
		using other = PMemoryPool<U>;
	};

public:
	PMemoryPool(Alloc_pointer alloc = nullptr) noexcept
	{
		if (alloc != nullptr)
		{
			this->Alloc = alloc;
			this->SelfManagement = false;
		}
		else
		{
			this->Alloc = new PMemoryAlloc_Default();
			this->SelfManagement = true;
		}
	}

	PMemoryPool(PMemoryAlloc_Default* alloc) noexcept
	{
		PMemoryPool::PMemoryPool(dynamic_cast<Alloc_pointer>(alloc));
	}

	~PMemoryPool() noexcept
	{
		if (this->SelfManagement)
		{
			delete Alloc;
		}
	}

	virtual pointer allocate(size_type size, const_unknown_pointer ptr = nullptr) override final
	{
		return reinterpret_cast<pointer>(
			this->Alloc->alloc(
				size * sizeof(value_type)));
	}

	virtual void deallocate(pointer ptr, size_t size) override final
	{
		this->Alloc->dealloc(
			reinterpret_cast<unknown_pointer>(ptr),
			size * sizeof(value_type)
		);
	}

	virtual void construct(pointer ptr, const_reference value) override final
	{
		//placement new
		new (ptr) value_type(value);
	}

	template<typename ...Args>
	void construct(pointer ptr, Args&& ...args)
	{
		//placement new
		new (ptr) value_type(std::forward<Args>(args)...);
	}

	virtual void destroy(pointer ptr) override final
	{
		ptr->~value_type();
	}

	virtual pointer address(reference object) override final
	{
		return static_cast<pointer>(&object);
	}

	virtual const_pointer const_address(const_reference object) override final
	{
		return static_cast<const_pointer>(&object);
	}

protected:
	Alloc_pointer Alloc;
	bool SelfManagement;
};

#endif // !PMEMORYALLOCATOR