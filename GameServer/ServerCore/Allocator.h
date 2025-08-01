#pragma once

/*-------------------
	BaseAllocator
-------------------*/

class BaseAllocator
{
public:
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);
};

/*-------------------
	StompAllocator
-------------------*/

// StompAllocator: 4����Ʈ�� ����Ѵٰ� �ص� ū ������ ��Ƽ�
// ���ߴܰ�(Debug)���� ���׸� ã�� ���� ���

class StompAllocator
{
	enum { PAGE_SIZE = 0x400 }; // 0x1000(16����) = 4096


public:
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);
};

/*-------------------
	PoolAllocator
-------------------*/

class PoolAllocator
{
public:
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);
};


/*-------------------
	STL Allocator
-------------------*/

template<typename T>
class StlAllocator
{
public:
	using value_type = T;

	StlAllocator() { }

	template<typename Other>
	StlAllocator(const StlAllocator<Other>&) { }

	T* allocate(size_t count)
	{
		const int32 size = static_cast<int32>(count * sizeof(T)); // ���⼭ count�� vector���� size(����)
 		return static_cast<T*>(PoolAllocator::Alloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		PoolAllocator::Release(ptr);
	}

};