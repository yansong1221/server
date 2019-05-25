#pragma once

#include "../CoreService/TCPManager.h"
#include "../CoreService/EventDispatcher.h"
#include "../CoreService/NetPacket.h"
#include "../MessageDefine/MessageCmd.h"

class CCenterServer : ITCPEventDelegate
{
public:
	CCenterServer();
	~CCenterServer();

public:
	virtual void onNetworkMessage(int nConnID, CNetPacket* pNetPacket);
	virtual void onConnected(int nConnID);
	virtual void onDisconnected(int nConnID);

	void onMessageRegisterServer(int nConnID, CNetPacket* pNetPacket);
public:
	void run();

private:
	CEventDispatcher m_EventDispatcher;
	CTCPManager m_TCPManager;
};