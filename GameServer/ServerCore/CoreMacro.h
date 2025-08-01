#pragma once

#define OUT // OUT�� ���� �ٲ� �� �ִ� ��Ʈ�� �ش�


/* ----------------------
		  Lock
---------------------- */

#define USE_MANY_LOCKS(count)	Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define READ_LOCK_IDX(idx)		ReadLockGuard readLockGuard_##idx(_locks[idx], typeid(this).name()); // ##idx�� ������ 1�� �ٲ�
#define READ_LOCK				READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx)		WriteLockGuard writeLockGuard_##idx(_locks[idx], typeid(this).name()); // ##idx�� ������ 1�� �ٲ�
#define WRITE_LOCK				WRITE_LOCK_IDX(0)




/* ----------------------
		  Crash
---------------------- */

// �츮�� ���������� CRASH�� ������ ���� ���̴�
// - #define�� ���� ���� �ϱ� ���ؼ��� \ �� �ٸ��� �߰�
// - __analysis_assume�� ����ؼ� VS���� crash�� nullptr�� �ƴ϶�� �����ض�! �˷���
// - ��, Ư�� ������ �׻� ���̶�� �����ϵ��� �����ϴ� ��!
// - �˷����� �ʴ´ٸ� *crash = 0xDEADBEFF; �� ����ϱ� ������ VS�� ������ �ܰ迡�� ������ ���ͼ� ������ �ȵȴ�

#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEFF;					\
}

// ���Ǻ� Crash
#define ASSERT_CRASH(expr)					\
{											\
	if(!(expr))								\
	{										\
		__analysis_assume(expr);			\
		CRASH("ASSERT_CRASH");				\
	}										\
}