#pragma once

/*
	MemoryPool
	1) 크기가 같은 데이터들을 모아 놓은 MemoryPool [[32][32][32][32]]
	2) 하나의 큰 크기에서 따로따로 모아 놓은 MemoryPool [[32][64][100][128]]
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

	// 헤더기입 함수
	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size);				// placement new
		return reinterpret_cast<void*>(++header);	// Data 위치로 반납
	}

	// 헤더반납 함수
	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize; // Data의 크기가 얼마인지

	// TODO : 필요한 추가 정보(typeinfo.. 등 여러 개의 정보를 넣어서 디버깅에 좋다)
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
	int32			_allocSize = 0;		// 각각의 MemoryPool이 담을 수 잇는 allocSize(MemoryPool [[32][32][32][32]])
	atomic<int32>	_useCount = 0;		// 몇명이 이 MemoryPool을 사용하는지
	atomic<int32>	_reserveCount = 0;	// MemoryPool에 저장이 되는 총 개수


};