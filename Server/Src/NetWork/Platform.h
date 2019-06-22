#pragma once

#define PLATFORM_WIN		0
#define PLATFORM_LINUX		1

//platform define
#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#define PLATFORM_TYPE		PLATFORM_WIN
#else
#define PLATFORM_TYPE		PLATFORM_LINUX
#endif // _WIN32

//win header
#if PLATFORM_TYPE == PLATFORM_WIN
#include <WinSock2.h>
#include <windows.h>
#include <MSWSock.h>
#pragma comment(lib, "WS2_32")
#endif

//Linux header
#if PLATFORM_TYPE == PLATFORM_LINUX
#include <errno.h>
#include <float.h>
#include <pthread.h>	
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <iconv.h>
#include <langinfo.h>   /* CODESET */
#include <stdint.h>
#include <signal.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <tr1/unordered_map>
#include <tr1/functional>
#include <tr1/memory>
#include <linux/types.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/resource.h> 
#include <linux/errqueue.h>
#endif

//common header
#include <string>
#include <cstdint>
#include <cassert>
#include <functional>
#include <unordered_map>
#include <iostream>
//typedef
#if PLATFORM_TYPE == PLATFORM_LINUX
typedef  int SOCKET;
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET  (SOCKET)(~0)

typedef struct _WSABUF {
	size_t len;     /* the length of the buffer */
	char *buf; /* the pointer to the buffer */
} WSABUF, FAR * LPWSABUF;

#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))

#endif


enum IO_OPERATION_TYPE
{
	NULL_POSTED,		// ���ڳ�ʼ����������
	ACCEPT_POSTED,		// Ͷ��Accept����
	SEND_POSTED,		// Ͷ��Send����
	RECV_POSTED,		// Ͷ��Recv����
};

class IOContext
{
public:

#if PLATFORM_TYPE == PLATFORM_WIN
	WSAOVERLAPPED		overLapped;			// ÿ��socket��ÿһ��IO��������Ҫһ���ص��ṹ
#endif

	WSABUF				wsaBuf;				// ���ݻ���

	SOCKET				fd;					// ��IO������Ӧ��socket	
	IO_OPERATION_TYPE	ioType;				// IO��������

	IOContext()
	{
		reset();
	}

	void reset()
	{
#if PLATFORM_TYPE == PLATFORM_WIN
		ZeroMemory(&overLapped, sizeof(overLapped));
#endif
		fd = INVALID_SOCKET;
		ZeroMemory(&wsaBuf, sizeof(wsaBuf));
		ioType = NULL_POSTED;
	}
};