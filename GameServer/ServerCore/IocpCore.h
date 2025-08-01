#pragma once

/*------------------------------
	IocpObject
	- IOCP에 등록할 Object(key)
------------------------------*/


// enable_shared_from_this<>
// -> 내부적으로 weak_ptr<>를 가지고 있음 
// -> 즉, 자기 자신을 가질 수 있게 해준다
// -> 대신에, IocpObject 객체에 대해 생으로 생성하지말고 shared_ptr<>로 생성해야함
class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE	GetHandle() abstract;
	virtual void	Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};

/*--------------
	IocpCore
---------------*/

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE		GetHandle() { return _iocpHandle; }

	bool		Register(IocpObjectRef iocpObject);			// IOCP에 일감 등록함수
	bool		Dispatch(uint32 timeoutMs = INFINITE);		// IOCP에 일감이 있나 확인

private:
	HANDLE		_iocpHandle;
};