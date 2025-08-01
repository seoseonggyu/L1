#pragma once
#include "Allocator.h"

class MemoryPool;

/*--------------------------------------
	Memory(MemoryPool�� �Ѱ��ϴ� ����)
--------------------------------------*/

class Memory
{
	enum
	{
		// ~1024Byte���� 32Byte����, ~2048Byte���� 128Byte����, ~4096Byte���� 256Byte����
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	Memory();
	~Memory();

	void*	Allocate(int32 size);
	void	Release(void* ptr);

private:
	vector<MemoryPool*> _pools;

	// �޸� ũ�� <-> �޸� Ǯ
	// O(1) ������ ã�� ���� ���̺�
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
};

// ����� �ڵ�(p671)
template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
	new(memory)Type(forward<Args>(args)...); // new(memory)Type(): memory�� �����ϱ� �޸𸮸� ���������� �����ڸ� �������! 
	// placement new
	// placement new(�޸𸮸� �̸� ������ �Ҵ��ϰ� �޸� �����ٰ� ��ü ������ ȣ��)
	// �츮�� ����ϴ� �Ϲ� new�� �޸𸮸� �������ڸ��� �ٷ� �޸� �����ٰ� ������ ȣ��
	// �츮�� ���� xnew�� �޸𸮸� �����! ���� ������ �ϳ� ����� �κ��� �����ؾ� �Ѵ�.
	// �׷���, �����ڸ� � �Ÿ� ȣ���ؾ��ұ�?
	// ������ ���ڰ� �ϳ�, �� Ȥ�� �� �̻��� ���� ��쿡�� �����?
	// �׷� ��Ȳ�� ����ؼ� ���°� ��������
	// �׷���, ���ڰ� �ް� ���� ���������� �𸣱� ������
	// ���������� �Ѱ��ش�!
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();
	PoolAllocator::Release(obj);
}

template<typename Type, typename... Args>
shared_ptr<Type> MakeShared(Args&&... args)
{
	return shared_ptr<Type>{ xnew<Type>(forward<Args>(args)...), xdelete<Type> };
}