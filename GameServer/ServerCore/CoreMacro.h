#pragma once

#define OUT // OUT은 값이 바뀔 수 있는 힌트를 준다


/* ----------------------
		  Lock
---------------------- */

#define USE_MANY_LOCKS(count)	Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define READ_LOCK_IDX(idx)		ReadLockGuard readLockGuard_##idx(_locks[idx], typeid(this).name()); // ##idx가 저절로 1로 바뀜
#define READ_LOCK				READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx)		WriteLockGuard writeLockGuard_##idx(_locks[idx], typeid(this).name()); // ##idx가 저절로 1로 바뀜
#define WRITE_LOCK				WRITE_LOCK_IDX(0)




/* ----------------------
		  Crash
---------------------- */

// 우리가 인위적으로 CRASH를 내려고 만든 것이다
// - #define을 여러 줄을 하기 위해서는 \ 을 줄마다 추가
// - __analysis_assume을 사용해서 VS에게 crash가 nullptr이 아니라고 가정해라! 알려줌
// - 즉, 특정 조건이 항상 참이라고 가정하도록 지시하는 것!
// - 알려주지 않는다면 *crash = 0xDEADBEFF; 를 사용하기 때문에 VS가 컴파일 단계에서 오류가 나와서 실행이 안된다

#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEFF;					\
}

// 조건부 Crash
#define ASSERT_CRASH(expr)					\
{											\
	if(!(expr))								\
	{										\
		__analysis_assume(expr);			\
		CRASH("ASSERT_CRASH");				\
	}										\
}