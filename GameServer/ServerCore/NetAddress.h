#pragma once

/*--------------
	NetAddress
---------------*/

/*
	얘가 하는 것은 나중에 SOCKADDR_IN을 통해 클라에 주소를 받는데
	해당 클라에 IP주소 같은 것들을 추출하는 경우가 있는데 그때마다
	함수를 매번 부르기 보다는 모든 주소를 NetAddress를 통해 관리해서
	정보를 추출하게 유도하는 것이다							
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

