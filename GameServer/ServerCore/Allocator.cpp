#include "pch.h"
#include "Allocator.h"
#include "Memory.h"

/*-------------------
	BaseAllocator
-------------------*/

void* BaseAllocator::Alloc(int32 size)
{
	return ::malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
	::free(ptr);
}

/*-------------------
	StompAllocator
-------------------*/

void* StompAllocator::Alloc(int32 size)
{
	// 어차피 자동으로 페이지 크기에 맞춰서 메모리를 반환하지만
	// 그래도 우리가 요청하는 크기를 명확하게
	// 코드로 작성하는게 낫기 때문에 size를 넣지않고 PAGE_SIZE의 배수로 메모리 요청을 하는 것이다.

	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	// 우리가 사용할 메모리보다 큰 영역을 잡아준다 // [[사용할영역]         ]

	// [         [사용할영역]] 사용할영역을 뒤로 잡아서 오버플로우 문제 해결 
	const int64 dataOffset = pageCount * PAGE_SIZE - size;
	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
}

void StompAllocator::Release(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);
	const int64 baseAddress = address - (address % PAGE_SIZE);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}

/*-------------------
	PoolAllocator
-------------------*/

void* PoolAllocator::Alloc(int32 size)
{
	return GMemory->Allocate(size);
}

void PoolAllocator::Release(void* ptr)
{
	GMemory->Release(ptr);
}
