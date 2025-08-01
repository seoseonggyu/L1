#pragma once

/*
	MemoryPool
	1) ũ�Ⱑ ���� �����͵��� ��� ���� MemoryPool [[32][32][32][32]]
	2) �ϳ��� ū ũ�⿡�� ���ε��� ��� ���� MemoryPool [[32][64][100][128]]
*/

enum
{
	SLIST_ALIGNMENT = 16
};

/*-----------------
	MemoryHeader
------------------*/

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
	// [MemoryHeader][Data]
	MemoryHeader(int32 size) : allocSize(size) { }

	// ������� �Լ�
	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size);				// placement new
		return reinterpret_cast<void*>(++header);	// Data ��ġ�� �ݳ�
	}

	// ����ݳ� �Լ�
	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize; // Data�� ũ�Ⱑ ������

	// TODO : �ʿ��� �߰� ����(typeinfo.. �� ���� ���� ������ �־ ����뿡 ����)
};


/*-----------------
	MemoryPool
------------------*/

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void			Push(MemoryHeader* ptr);
	MemoryHeader*	Pop();


private:
	SLIST_HEADER	_header;
	int32			_allocSize = 0;		// ������ MemoryPool�� ���� �� �մ� allocSize(MemoryPool [[32][32][32][32]])
	atomic<int32>	_useCount = 0;		// ����� �� MemoryPool�� ����ϴ���
	atomic<int32>	_reserveCount = 0;	// MemoryPool�� ������ �Ǵ� �� ����


};