#pragma once

/*--------------------------------------------
	RecvBuffer
	- TCP의 특성인 데이터 경계에 대해서
	- 데이터가 한번에 완전체로 오지 않을 경우가
	- 발생한다. 그러면 완전체로 도착하지 않았다고
	- 할 경우에 기존에 있던 데이터를 남겨주도
	- 뒤에 이어서 덧붙여가지고 데이터를 복사하는
	- 방식에 RecvBuffer를 만들자.
--------------------------------------------*/

// [][r][][][] [][][w][][] [][][][][] [][][][][] [][][][][]

class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };

public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();

	void			Clean();
	bool			OnRead(int32 numOfBytes);
	bool			OnWrite(int32 numOfBytes);

	BYTE*			ReadPos() { return &_buffer[_readPos]; }
	BYTE*			WritePos() { return &_buffer[_writePos]; }
	int32			DataSize() { return _writePos - _readPos; }
	int32			FreeSize() { return _capacity - _writePos; }

private:
	int32			_capacity = 0;
	int32			_bufferSize = 0;
	int32			_readPos = 0;
	int32			_writePos = 0;
	Vector<BYTE>	_buffer;
};

