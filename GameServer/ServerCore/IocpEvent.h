#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	//PreRecv, // 0byte recv(고급기법.. 현재는 아직 사용하지 않음)
	Recv,
	Send
};

/*-----------------------------------------------------
	IocpEvent
	- 어떤 용도(recv, send..)로 넘겨줄지 정리
	- virtual를 사용하면 안댐
	  why? 메모리 맨 앞부분에 virtable이 만들어지니까
	  OVERLAPPED에 관한 것이 아니라 virtable이 생성되서..
	  따라서, offset 0에 OVERLAPPED에 관한 것이 있음
-----------------------------------------------------*/

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void			Init();

public:
	EventType		eventType;
	IocpObjectRef	owner;		// 자신의 주인을 기억(key를 기억)
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
	// 이렇게 잡는 이유는 WSASend를 하는 순간에 데이터가 날아가지
	// 않게 reference를 유지하기 위해서
};