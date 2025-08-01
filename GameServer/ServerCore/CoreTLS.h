#pragma once
#include <stack>

// extern: �ٸ� ������ ���� ������ ���� ����
// static: �ܺ� ���Ͽ��� ���� �Ұ���

// �ٸ� ������ CoreTLS.h ������ �����ϸ� extern�� ���� CoreTLS.cpp�� ���������� ���� ����
extern thread_local uint32				LThreadId;
extern thread_local uint64				LEndTickCount;

extern thread_local std::stack<int32>	LLockStack;
extern thread_local SendBufferChunkRef	LSendBufferChunk;
extern thread_local	class JobQueue*		LCurrentJobQueue;