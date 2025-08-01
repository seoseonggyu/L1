#include "pch.h"
#include "NetAddress.h"

/*--------------
	NetAddress
---------------*/

NetAddress::NetAddress(SOCKADDR_IN sockAddr) : _sockAddr(sockAddr)
{
}

NetAddress::NetAddress(wstring ip, uint16 port)
{
	::memset(&_sockAddr, 0, sizeof(_sockAddr));
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr = Ip2Address(ip.c_str());
	_sockAddr.sin_port = ::htons(port);
}

wstring NetAddress::GetIpAddress()
{
	WCHAR buffer[100];
	::InetNtopW(AF_INET, &_sockAddr.sin_addr, buffer, len32(buffer));
	return wstring(buffer);
}


// 나중에 dns(www.naver.com)를 ip주소로 환산하능 기능용
IN_ADDR NetAddress::Ip2Address(const WCHAR* ip)
{
	// ip를 받아서 IN_ADDR를 만들어준다
	IN_ADDR address;
	::InetPtonW(AF_INET, ip, &address);
	return address;
}
