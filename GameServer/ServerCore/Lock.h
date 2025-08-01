#pragma once

/*-------------------
	RW SpinLock
-------------------*/

/*-----------------------------------------------
[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W : WriteFlag (Exclusive Lock Owner ThreadId)
R : ReadFlag (Shared Lock Count)
-----------------------------------------------*/

// �ϳ��� �����忡��			W -> W (O)
// �ϳ��� �����忡��			W -> R (O)
// �ϳ��� �����忡��			W -> R -> W (X)
// �ϳ��� �����忡��			W -> R -> R (O)
// �ϳ� Ȥ�� ���� �����忡��	R -> W (X)
// �ϳ� Ȥ�� ���� �����忡��	R -> R (O)

class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,		// �ִ� ��ٷ��� ƽ
		MAX_SPIN_COUNT = 5000,				// ������ �ִ� �� ��
		WRITE_THREAD_MASK = 0xFFFF'0000,	// ���� ��Ʈ �÷��� ���
		READ_COUNT_MASK = 0x0000'FFFF,		// ���� ��Ʈ �÷��� ���
		EMPTY_FLAG = 0x0000'0000
	};

public:
	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);

private:
	Atomic<uint32>	_lockFlag = EMPTY_FLAG;
	uint16			_writeCount = 0; // ����(�ϳ��� �����常) ����ϴ� �� -> atomic �ʿ� X
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