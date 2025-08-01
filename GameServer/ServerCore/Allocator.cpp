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
	// ������ �ڵ����� ������ ũ�⿡ ���缭 �޸𸮸� ��ȯ������
	// �׷��� �츮�� ��û�ϴ� ũ�⸦ ��Ȯ�ϰ�
	// �ڵ�� �ۼ��ϴ°� ���� ������ size�� �����ʰ� PAGE_SIZE�� ����� �޸� ��û�� �ϴ� ���̴�.

	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	// �츮�� ����� �޸𸮺��� ū ������ ����ش� // [[����ҿ���]         ]

	// [         [����ҿ���]] ����ҿ����� �ڷ� ��Ƽ� �����÷ο� ���� �ذ� 
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
