#pragma once

/*------------------------------
	IocpObject
	- IOCP�� ����� Object(key)
------------------------------*/


// enable_shared_from_this<>
// -> ���������� weak_ptr<>�� ������ ���� 
// -> ��, �ڱ� �ڽ��� ���� �� �ְ� ���ش�
// -> ��ſ�, IocpObject ��ü�� ���� ������ ������������ shared_ptr<>�� �����ؾ���
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

	bool		Register(IocpObjectRef iocpObject);			// IOCP�� �ϰ� ����Լ�
	bool		Dispatch(uint32 timeoutMs = INFINITE);		// IOCP�� �ϰ��� �ֳ� Ȯ��

private:
	HANDLE		_iocpHandle;
};