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

//typedef
#if PLATFORM_TYPE == PLATFORM_LINUX
typedef  int SOCKET;
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET  (SOCKET)(~0)
#endif
