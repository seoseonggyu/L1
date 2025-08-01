#pragma once

/*-------------------
	RW SpinLock
-------------------*/

/*-----------------------------------------------
[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W : WriteFlag (Exclusive Lock Owner ThreadId)
R : ReadFlag (Shared Lock Count)
-----------------------------------------------*/

// 하나의 쓰레드에서			W -> W (O)
// 하나의 쓰레드에서			W -> R (O)
// 하나의 쓰레드에서			W -> R -> W (X)
// 하나의 쓰레드에서			W -> R -> R (O)
// 하나 혹은 여러 쓰레드에서	R -> W (X)
// 하나 혹은 여러 쓰레드에서	R -> R (O)

class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,		// 최대 기다려줄 틱
		MAX_SPIN_COUNT = 5000,				// 스핀을 최대 몇 번
		WRITE_THREAD_MASK = 0xFFFF'0000,	// 상위 비트 플래그 방식
		READ_COUNT_MASK = 0x0000'FFFF,		// 하위 비트 플래그 방식
		EMPTY_FLAG = 0x0000'0000
	};

public:
	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);

private:
	Atomic<uint32>	_lockFlag = EMPTY_FLAG;
	uint16			_writeCount = 0; // 나만(하나의 쓰레드만) 사용하는 것 -> atomic 필요 X
};

/*-------------------
  LockGuards (RAII)
-------------------*/

class ReadLockGuard {
public:
	ReadLockGuard(Lock& lock, const char* name)
		: _lock(lock), _name(name) {
		_lock.ReadLock(name);
	}
	~ReadLockGuard() { _lock.ReadUnlock(_name); }

private:
	Lock& _lock;
	const char* _name;

};

class WriteLockGuard {
public:
	WriteLockGuard(Lock& lock, const char* name)
		: _lock(lock), _name(name) {
		_lock.WriteLock(name);
	}
	~WriteLockGuard() { _lock.WriteUnlock(_name); }

private:
	Lock& _lock;
	const char* _name;
};