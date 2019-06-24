#include "TCPConnector.h"
#include "CommonFunc.h"



TCPConnector::TCPConnector()
{
}


TCPConnector::~TCPConnector()
{
}

uint32_t TCPConnector::connetTo(const std::string& address, int port)
{
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(address.c_str());

	SOCKET fd = CommonFunc::createSocket();
	if (connect(fd, (const sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		CommonFunc::closeSocket(fd);
		return 0;
	}


}
