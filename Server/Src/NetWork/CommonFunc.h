#pragma once

#include "Platform.h"

class CommonFunc
{
public:
	CommonFunc();
	~CommonFunc();

public:
	static SOCKET createSocket();
	static void closeSocket(SOCKET fd);
	static void shutdownSocket(SOCKET fd);
};

