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

// StompAllocator: 4바이트를 사용한다고 해도 큰 영역을 잡아서
// 개발단계(Debug)에서 버그를 찾기 위해 사용

class StompAllocator
{
	enum { PAGE_SIZE = 0x400 }; // 0x1000(16진수) = 4096


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
		const int32 size = static_cast<int32>(count * sizeof(T)); // 여기서 count는 vector같은 size(개수)
 		return static_cast<T*>(PoolAllocator::Alloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		PoolAllocator::Release(ptr);
	}

};