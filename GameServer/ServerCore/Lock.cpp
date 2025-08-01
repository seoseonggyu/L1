#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "DeadLockProfiler.h"

void Lock::WriteLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif

	// 동일한 쓰레드가 소유하고 있다면 무조건 성공
	// -> 재귀적으로 가능하게
	// - 추가적으로, ReadLock를 잡았을 경우에는 안된다!
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (lockThreadId == LThreadId) {
		++_writeCount;
		return;
	}

	// 아무도 소유(수정) 및 공유(읽는 것)하고 있지 않을 때, 경합해서 소유권을 얻는다
	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	while (true) {

		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; ++spinCount) {
			uint32 expected = EMPTY_FLAG;
			
			// 내가 처음부터 잡거나, 락을 아무도 사용하지 않는 경우에 락을 잡는다
			if (_lockFlag.compare_exchange_strong(OUT expected, desired)) {
				++_writeCount;
				// 우리가 만든 락은 재귀적으로 같은 쓰레드가 WriteLock을 획득할 수 있게 만듬
				// -> 따라서 _writeCount를 늘려서 한번 더 잡게 해준다 
				return;
			}
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	// ReadLock을 다 풀기 전에는 WriteUnlock 불가능
	// 즉, W->R->W 가 안되는 것임
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0) {
		CRASH("INVALID_UNLOCK_ORDER");
	}

	const int32 lockCount = --_writeCount;
	if (lockCount == 0)
		_lockFlag.store(EMPTY_FLAG);
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	// 동일한 쓰레드가 소유하고 있다면 무조건 성공
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (lockThreadId == LThreadId) {
		_lockFlag.fetch_add(1);
		return;
	}

	// 아무도 소유(Write)하고 있지 않을 때 경합해서 공유 카운터를 올린다
	const int64 beginTick = ::GetTickCount64();
	while (true) {

		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; ++spinCount) {
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif

	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK");
}
