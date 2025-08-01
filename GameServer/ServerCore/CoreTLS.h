#pragma once
#include <stack>

// extern: 다른 파일의 전역 변수에 접근 가능
// static: 외부 파일에서 접근 불가능

// 다른 곳에서 CoreTLS.h 파일을 선언하면 extern이 들어가서 CoreTLS.cpp의 전역변수에 접근 가능
extern thread_local uint32				LThreadId;
extern thread_local uint64				LEndTickCount;

extern thread_local std::stack<int32>	LLockStack;
extern thread_local SendBufferChunkRef	LSendBufferChunk;
extern thread_local	class JobQueue*		LCurrentJobQueue;