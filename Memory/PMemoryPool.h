#pragma once
#ifndef PMEMORYPOOL
#define PMEMORYPOOL

#include <cstddef>
//#include <cstdlib>//malloc,free
#include <cstdint>
#include <new>//oparator new,operator delete,placement new,在尚未处理的的(raw)内存构造一个对象
#include <cassert>
#include <stack>
#include <memory>
#include <memory_resource> // c++17

using byte_type = uint8_t;
using size_type = size_t;
using unknown_type = void;
using data_pointer = byte_type*;
using const_reference_size_type = const size_t&;
using unknown_pointer = void*;
using const_unknown_pointer = const void*;

#define _1K      size_t(1024u)
#define _2K      size_t(2048u)
#define _4K      size_t(4096u)
#define _8K      size_t(8192u)
#define _16K     size_t(16384u)
#define _32K     size_t(32768u)
#define _64K     size_t(65536u)
#define _128K    size_t(131072u)
#define _256K    size_t(262144u)
#define _512K    size_t(524288u)
#define _1024K   size_t(1048576u)
#define _1M		 _1024K
#define _2M      size_t(2097152u)
#define _4M      size_t(4194304u)
#define _8M      size_t(8388608u)
#define _16M     size_t(16777216u)
#define _32M     size_t(33554432u)
#define _64M     size_t(67108864u)
#define _128M    size_t(134217728u)
#define _256M    size_t(268435456u)
#define _512M    size_t(536870912u)
#define _1024M   size_t(1073741824u)
#define _1G		 _1024M

/* 内部内存分配接口 */
class Ialloc
{
public:
	virtual ~Ialloc() {}

	virtual unknown_pointer alloc(size_type size) = 0;
	virtual void dealloc(unknown_pointer ptr, size_type size) = 0;
};
using Alloc_pointer = Ialloc*;

using options = std::pmr::pool_options;
using base_buffer = std::pmr::monotonic_buffer_resource;
using base_pool = std::pmr::unsynchronized_pool_resource;


class new_deleteAlloc :public Ialloc
{
public:
	new_deleteAlloc();

	virtual unknown_pointer alloc(size_type size) override final;

	virtual void dealloc(unknown_pointer ptr, size_type size) override final;

protected:
	std::unique_ptr<base_buffer> resource;
};


/* 内部内存分配器:池化内存 */
class PMemoryAlloc_Default :public Ialloc
{
public:
	/* 根据实际设置尺寸更合适 */
	PMemoryAlloc_Default(
		const_reference_size_type Level1_Range = _512K,
		const_reference_size_type Level2_Range = _4M,
		const_reference_size_type Level3_Range = _32M,
		const_reference_size_type Level4_Range = _256M,
		const options& L1_op = options{ _2K,_64K },
		const options& L2_op = options{ _4K,_512K },
		const options& L3_op = options{ _16K,_4M },
		const options& L4_op = options{ _32K,_64M });

	~PMemoryAlloc_Default();

	virtual unknown_pointer alloc(size_type size) override final;

	virtual void dealloc(unknown_pointer ptr, size_type size) override final;

	base_pool* GetL1Pool();
	base_pool* GetL2Pool();
	base_pool* GetL3Pool();
	base_pool* GetL4Pool();

protected:
	std::unique_ptr<base_pool> L1_pool;
	std::unique_ptr<base_pool> L2_pool;
	std::unique_ptr<base_pool> L3_pool;
	std::unique_ptr<base_pool> L4_pool;

	std::unique_ptr<base_buffer> L1_buffer;
	std::unique_ptr<base_buffer> L2_buffer;
	std::unique_ptr<base_buffer> L3_buffer;
	std::unique_ptr<base_buffer> L4_buffer;

	size_type L1;
	size_type L2;
	size_type L3;
	size_type L4;

	options L1_op;
	options L2_op;
	options L3_op;
	options L4_op;

	std::unique_ptr<unknown_type> raw_data;
};


/* 频繁创建销毁的页面数据,大小相同,归还顺序必须一致 */
class PMemoryAlloc_PageTemporaryData :public Ialloc
{
public:
	PMemoryAlloc_PageTemporaryData(
		const_reference_size_type PageSize,
		const_reference_size_type numPageCaches);

	~PMemoryAlloc_PageTemporaryData();

	/* size = PageSize */
	virtual unknown_pointer alloc(size_type size) override final;

	/* size = PageSize */
	virtual void dealloc(void* ptr, size_type size) override final;

protected:
	void BuildPages();

protected:
	unknown_pointer address;
	unknown_pointer allMemPagesHead;

	size_type FreePageIndex;

	size_type PageSize;
	size_type numPageCaches;
};


/* 大量同等大小的实例数据分配与销毁,比如粒子效果 */
class PMemoryAlloc_InstanceData :public Ialloc
{
public:
	PMemoryAlloc_InstanceData(
		Alloc_pointer Alloc,
		const_reference_size_type InstanceSize,
		const_reference_size_type initNum = 128u);

	~PMemoryAlloc_InstanceData();

	void InsertInstances(size_type Num);

	virtual unknown_pointer alloc(size_type size) override final;

	virtual void dealloc(unknown_pointer ptr, size_type size) override final;

protected:
	size_type InstanceSize;

	unknown_pointer address;
	Alloc_pointer Alloc;
	std::stack<unknown_pointer> Instances;
};

//-------------------------------------------------------------------------------------------------------------------
inline PMemoryAlloc_Default::PMemoryAlloc_Default(
	const_reference_size_type Level1_Range,
	const_reference_size_type Level2_Range,
	const_reference_size_type Level3_Range,
	const_reference_size_type Level4_Range,
	const options& L1_op,
	const options& L2_op,
	const options& L3_op,
	const options& L4_op)
{
	assert(Level1_Range >= _16K && "L1 too small.");
	assert(Level1_Range * 8u <= Level2_Range && "L2 too small.");
	assert(Level2_Range * 8u <= Level3_Range && "L3 too small.");
	assert(Level3_Range * 8u <= Level4_Range && "L4 too small.");

	L1 = Level1_Range;
	L2 = Level2_Range;
	L3 = Level3_Range;
	L4 = Level4_Range;

	this->L1_op = L1_op;
	this->L2_op = L2_op;
	this->L3_op = L3_op;
	this->L4_op = L4_op;

	raw_data.reset(operator new(L4 + L3 + L2 + L1));
	data_pointer ptr = static_cast<data_pointer>(raw_data.get());

	//build buffers
	ptr += L1 + L2 + L3;
	L4_buffer = std::make_unique<base_buffer>(static_cast<unknown_pointer>(ptr), L4);
	ptr -= L3;
	L3_buffer = std::make_unique<base_buffer>(static_cast<unknown_pointer>(ptr), L3, L4_buffer);
	ptr -= L2;
	L2_buffer = std::make_unique<base_buffer>(static_cast<unknown_pointer>(ptr), L2, L3_buffer);
	ptr -= L1;
	L1_buffer = std::make_unique<base_buffer>(static_cast<unknown_pointer>(ptr), L1, L2_buffer);

	//build pools

	L1_pool = std::make_unique<base_pool>(L1_op, L1_buffer.get());
	L2_pool = std::make_unique<base_pool>(L2_op, L2_buffer.get());
	L3_pool = std::make_unique<base_pool>(L3_op, L3_buffer.get());
	L4_pool = std::make_unique<base_pool>(L4_op, L4_buffer.get());
}

inline PMemoryAlloc_Default::~PMemoryAlloc_Default()
{
	//release pool
	L1_pool->release();
	L2_pool->release();
	L3_pool->release();
	L4_pool->release();
	//release buffer
	L1_buffer->release();
	L2_buffer->release();
	L3_buffer->release();
	L4_buffer->release();
	//release memory
	raw_data.reset();
}

inline unknown_pointer PMemoryAlloc_Default::alloc(size_type size)
{
	if (size <= L1_op.largest_required_pool_block)
	{
		return L1_pool->allocate(size);
	}
	else
		if (size <= L2_op.largest_required_pool_block)
		{
			return L2_pool->allocate(size);
		}
		else
			if (size <= L3_op.largest_required_pool_block)
			{
				return L3_pool->allocate(size);
			}
			else
				if (size <= L4_op.largest_required_pool_block)
				{
					return L4_pool->allocate(size);
				}
}

inline void PMemoryAlloc_Default::dealloc(unknown_pointer ptr, size_type size)
{
	if (size <= L1_op.largest_required_pool_block)
	{
		L1_pool->deallocate(ptr, size);
	}
	else
		if (size <= L2_op.largest_required_pool_block)
		{
			L2_pool->deallocate(ptr, size);
		}
		else
			if (size <= L3_op.largest_required_pool_block)
			{
				L3_pool->deallocate(ptr, size);
			}
			else
				if (size <= L4_op.largest_required_pool_block)
				{
					L4_pool->deallocate(ptr, size);
				}
}

inline base_pool* PMemoryAlloc_Default::GetL1Pool()
{
	return L1_pool.get();
}

inline base_pool* PMemoryAlloc_Default::GetL2Pool()
{
	return L2_pool.get();
}

inline base_pool* PMemoryAlloc_Default::GetL3Pool()
{
	return L3_pool.get();
}

inline base_pool* PMemoryAlloc_Default::GetL4Pool()
{
	return L4_pool.get();
}

PMemoryAlloc_PageTemporaryData::PMemoryAlloc_PageTemporaryData(const_reference_size_type PageSize, const_reference_size_type numPageCaches)
{
	assert(PageSize > 0u && "Wrong Page Size!");
	assert(numPageCaches > 0u && "Wrong Num Caches!");

	this->PageSize = PageSize;
	/* 双倍大小确保够用 */
	this->numPageCaches = numPageCaches * 2u;

	this->allMemPagesHead = nullptr;
	this->address = nullptr;
	this->FreePageIndex = 0u;
}

inline PMemoryAlloc_PageTemporaryData::~PMemoryAlloc_PageTemporaryData()
{
	operator delete(allMemPagesHead);
}

inline unknown_pointer PMemoryAlloc_PageTemporaryData::alloc(size_type size)
{
	assert(size == this->PageSize && "Wrong Page Size！");

	this->address = static_cast<unknown_pointer>(
		static_cast<data_pointer>(this->allMemPagesHead) + this->FreePageIndex);
	this->FreePageIndex = (this->FreePageIndex + this->PageSize) % (this->PageSize * this->numPageCaches);

	return this->address;
}

inline void PMemoryAlloc_PageTemporaryData::dealloc(void* ptr, size_type size)
{
	assert(size == this->PageSize && "Wrong Page Size！");
	/* do nothing */
}

inline void PMemoryAlloc_PageTemporaryData::BuildPages()
{
	this->allMemPagesHead = operator new(this->PageSize * this->numPageCaches);
}

PMemoryAlloc_InstanceData::PMemoryAlloc_InstanceData(Alloc_pointer Alloc, const_reference_size_type InstanceSize, const_reference_size_type initNum)
{
	assert(Alloc != nullptr && "Memory Allocator is nullptr!");
	assert(initNum > 0u && "Wrong Init Num!");

	this->Alloc = Alloc;
	this->InstanceSize = InstanceSize;
	this->address = nullptr;

	InsertInstances(initNum);
}

inline PMemoryAlloc_InstanceData::~PMemoryAlloc_InstanceData()
{
	while (!this->Instances.empty())
	{
		this->Alloc->dealloc(this->Instances.top(), this->InstanceSize);
		this->Instances.pop();
	}
}

inline void PMemoryAlloc_InstanceData::InsertInstances(size_type Num)
{
	this->address = this->Alloc->alloc(this->InstanceSize * Num);

	while (Num > 0u)
	{
		this->Instances.push(this->address);
		this->address = static_cast<unknown_pointer>(
			static_cast<data_pointer>(this->address) + this->InstanceSize);
		Num--;
	}
}

inline unknown_pointer PMemoryAlloc_InstanceData::alloc(size_type size)
{
	if (this->Instances.empty())
	{
		InsertInstances(32u);
	}

	this->address = this->Instances.top();
	this->Instances.pop();
	return this->address;
}

inline void PMemoryAlloc_InstanceData::dealloc(unknown_pointer ptr, size_type size)
{
	this->Instances.push(ptr);
}

new_deleteAlloc::new_deleteAlloc()
{
	resource = std::make_unique<base_buffer>(std::pmr::new_delete_resource);
}

inline unknown_pointer new_deleteAlloc::alloc(size_type size)
{
	return resource->allocate(size);
}

inline void new_deleteAlloc::dealloc(unknown_pointer ptr, size_type size)
{
	resource->deallocate(ptr, size);
}

#endif // !PMEMORYPOOL