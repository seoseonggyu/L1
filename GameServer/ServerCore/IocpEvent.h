#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	//PreRecv, // 0byte recv(��ޱ��.. ����� ���� ������� ����)
	Recv,
	Send
};

/*-----------------------------------------------------
	IocpEvent
	- � �뵵(recv, send..)�� �Ѱ����� ����
	- virtual�� ����ϸ� �ȴ�
	  why? �޸� �� �պκп� virtable�� ��������ϱ�
	  OVERLAPPED�� ���� ���� �ƴ϶� virtable�� �����Ǽ�..
	  ����, offset 0�� OVERLAPPED�� ���� ���� ����
-----------------------------------------------------*/

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void			Init();

public:
	EventType		eventType;
	IocpObjectRef	owner;		// �ڽ��� ������ ���(key�� ���)
};

/*----------------
	ConnectEvent
-----------------*/

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) { }
};

/*--------------------
	DisconnectEvent
----------------------*/

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) { }
};

/*----------------
	AcceptEvent
-----------------*/

class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) { }

public:
	SessionRef	session = nullptr;
};

/*----------------
	RecvEvent
-----------------*/

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) { }
};

/*----------------
	SendEvent
-----------------*/

class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) { }

	Vector<SendBufferRef> sendBuffers;
	// �̷��� ��� ������ WSASend�� �ϴ� ������ �����Ͱ� ���ư���
	// �ʰ� reference�� �����ϱ� ���ؼ�
};