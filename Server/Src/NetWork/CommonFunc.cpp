#include "CommonFunc.h"



CommonFunc::CommonFunc()
{
}


CommonFunc::~CommonFunc()
{
}

SOCKET CommonFunc::createSocket()
{
#if PLATFORM_TYPE == PLATFORM_WIN
	static bool init = false;
	if (!init)
	{
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		if (WSAStartup(sockVersion, &wsaData) != 0)
		{
			return SOCKET_ERROR;
		}
		init = true;
	}

	SOCKET fd = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
	SOCKET fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif

	if (fd == SOCKET_ERROR)
	{
		std::runtime_error("ϵͳ��Դ������� TCP/IP Э��û�а�װ");
	}
	return fd;
}

void CommonFunc::closeSocket(SOCKET fd)
{
#if PLATFORM_TYPE == PLATFORM_WIN
	closesocket(fd);
#else
	close(fd);
#endif
}

void CommonFunc::shutdownSocket(SOCKET fd)
{
	shutdown(fd, 0);
	shutdown(fd, 1);
}

void CommonFunc::clearSocket()
{
#if PLATFORM_TYPE == PLATFORM_WIN
	WSACleanup();
#endif
}
