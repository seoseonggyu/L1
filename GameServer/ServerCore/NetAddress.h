#pragma once

/*--------------
	NetAddress
---------------*/

/*
	�갡 �ϴ� ���� ���߿� SOCKADDR_IN�� ���� Ŭ�� �ּҸ� �޴µ�
	�ش� Ŭ�� IP�ּ� ���� �͵��� �����ϴ� ��찡 �ִµ� �׶�����
	�Լ��� �Ź� �θ��� ���ٴ� ��� �ּҸ� NetAddress�� ���� �����ؼ�
	������ �����ϰ� �����ϴ� ���̴�							
*/

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(wstring ip, uint16 port);

	SOCKADDR_IN&	GetSockAddr() { return _sockAddr; }
	wstring			GetIpAddress();
	uint16			GetPort() { return ::ntohs(_sockAddr.sin_port); }

public:
	static IN_ADDR	Ip2Address(const WCHAR* ip);

private:
	SOCKADDR_IN		_sockAddr = {};
};

