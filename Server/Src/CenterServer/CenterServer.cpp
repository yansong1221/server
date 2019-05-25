#include "CenterServer.h"
#include <iostream>

CCenterServer::CCenterServer()
	:m_TCPManager(m_EventDispatcher,*this)
{
	m_TCPManager.setAddress("127.0.0.1", 8800);
	m_TCPManager.listen();
	m_TCPManager.startAccept();

	m_EventDispatcher.getTimerManager().addTimer(1000, [this](CTimerItem* pTimerItem) 
	{
		std::cout << 1000 << std::endl;
	});

	m_EventDispatcher.getTimerManager().addTimer(1500, [this](CTimerItem* pTimerItem)
	{
		std::cout << 1500 << std::endl;
	});

	m_EventDispatcher.getTimerManager().addTimer(2500, [this](CTimerItem* pTimerItem)
	{
		std::cout << 2500 << std::endl;
	});

}

CCenterServer::~CCenterServer()
{

}

void CCenterServer::onNetworkMessage(int nConnID, CNetPacket* pNetPacket)
{
	switch (pNetPacket->getMsgHeader().unMainCmd)
	{
	case EMessageMainCmd::eMessageCenterServer:
	{
		switch (pNetPacket->getMsgHeader().unSubCmd)
		{
		case EMessageSubCenterServer::eMessageRegisterServer:
		{
			return onMessageRegisterServer(nConnID, pNetPacket);
		}
		default:
			break;
		}
	}
	default:
		break;
	}
}

void CCenterServer::onConnected(int nConnID)
{
	//m_TCPManager.sendData(nConnID, "helle", 6);
}

void CCenterServer::onDisconnected(int nConnID)
{

}

void CCenterServer::onMessageRegisterServer(int nConnID, CNetPacket* pNetPacket)
{
	CNetPacket packet;

	packet.getMsgHeader().unMainCmd = EMessageMainCmd::eMessageCenterServer;
	packet.getMsgHeader().unSubCmd = EMessageSubCenterServer::eMessageRegisterServerResult;

	m_TCPManager.sendData(nConnID, &packet);
}

void CCenterServer::run()
{
	m_EventDispatcher.run();
}
