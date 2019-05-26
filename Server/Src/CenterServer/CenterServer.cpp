#include "CenterServer.h"
#include <iostream>
#include "../CoreService/LogService.h"
#include "fmt/core.h"
#include "ServerManager.h"

CCenterServer::CCenterServer()
	:m_TCPManager(m_EventDispatcher,*this)
{

}

CCenterServer::~CCenterServer()
{

}

void CCenterServer::onNetworkMessage(int nConnID, CNetPacket* pNetPacket)
{
	SMsgHeader& msgHeader = pNetPacket->getMsgHeader();
	int32_t nMessageID = int32_t(msgHeader.unMainCmd << 16) | msgHeader.unSubCmd;

	auto iter = m_mapServerMessageCallBack.find(nMessageID);
	if (iter == m_mapServerMessageCallBack.end())
	{
		CLogService::get_mutable_instance().logWarning("没有注册返回的消息ID");
		return;
	}
	for (auto callBack : iter->second)
	{
		callBack(nConnID,pNetPacket);
	}
	return;
}

void CCenterServer::onConnected(int nConnID)
{
	//m_TCPManager.sendData(nConnID, "helle", 6);
}

void CCenterServer::onDisconnected(int nConnID)
{
	auto pGateServerInfo = CServerManager::get_mutable_instance().findServerByConnID(nConnID);
	if (pGateServerInfo)
	{
		CLogService::get_mutable_instance().logWarning(fmt::format("服务器与中心服务器断开连接[{}:{}],ConnID:{},ServerID:{},ServerType:{}", 
			pGateServerInfo->m_szServerAddress, 
			pGateServerInfo->m_nPort,
			pGateServerInfo->m_nConnID,
			pGateServerInfo->m_nServerID,
			pGateServerInfo->m_eServerType));


		//如果是网关掉线通知所有的登陆服务器
		if (pGateServerInfo->m_eServerType == SServerInfo::eServerTypeGate)
		{
			CMD::CenterServer::GateServerOffline gateServerOffline;
			gateServerOffline.nServerID = pGateServerInfo->m_nServerID;

			CNetPacket packet;
			packet.getMsgHeader().unMainCmd = (uint16_t)CMD::EMainCmd::eMessageCenterServer;
			packet.getMsgHeader().unSubCmd = (uint16_t)CMD::ESubCenterCmd::eMessageGateOffline;

			packet.getBody() << gateServerOffline;
			
			broadcastMessageToLoginServer(&packet);
		}
		
		CServerManager::get_mutable_instance().serverDisconnect(pGateServerInfo->m_nServerID);
		return;
	}
}

bool CCenterServer::startUp()
{
	CLogService::get_mutable_instance().logInfo("正在启动服务器...");

	if (listenLocalAddress() == false)
	{
		return false;
	}
	registerAllMessage();

	CLogService::get_mutable_instance().logInfo("服务器启动成功！");
	return true;
}

bool CCenterServer::listenLocalAddress()
{
	try
	{
		m_TCPManager.setAddress("127.0.0.1", 8800);
		m_TCPManager.listen();
		m_TCPManager.startAccept();

	}
	catch (const std::exception& e)
	{
		CLogService::get_mutable_instance().logInfo(fmt::format("监听本地端口[{}:{}]失败!错误消息:{}",
			"127.0.0.1",
			8800,
			e.what()));
		return false;
	}
	
	return true;
}

void CCenterServer::registerAllMessage()
{
	registerServerMessage((uint16_t)CMD::EMainCmd::eMessageCenterServer, (uint16_t)CMD::ESubCenterCmd::eMessageRegisterServer, std::bind(&CCenterServer::onMessageRegisterServer, this, std::placeholders::_1, std::placeholders::_2));
}

void CCenterServer::onMessageRegisterServer(int nConnID, CNetPacket* pNetPacket)
{
	try
	{
		CMD::CenterServer::RegisterServer registerServer;
		pNetPacket->getBody() >> registerServer;

		SServerInfo* pServerInfo = nullptr;

		if (registerServer.eServerType == CMD::CenterServer::RegisterServer::eServerTypeGateServer)
		{
			pServerInfo = CServerManager::get_mutable_instance().activateServer(
				SServerInfo::EServerType::eServerTypeGate, 
				nConnID, 
				registerServer.szServerAddress, 
				registerServer.n32Port);

			//通知给所有登陆服务器
			if (pServerInfo)
			{
				CNetPacket packet;
				packet.getMsgHeader().unMainCmd = (uint16_t)CMD::EMainCmd::eMessageCenterServer;
				packet.getMsgHeader().unSubCmd = (uint16_t)CMD::ESubCenterCmd::eMessageGateServerOnline;

				CMD::CenterServer::GateServerInfo gateServerInfo;

				strcpy(gateServerInfo.szServerAddress, registerServer.szServerAddress);
				gateServerInfo.n32Port = registerServer.n32Port;
				gateServerInfo.n32ServerID = pServerInfo->m_nServerID;

				packet.getBody() << gateServerInfo;

				broadcastMessageToLoginServer(&packet);
			}
			//通知给游戏服务器

		}
		else if (registerServer.eServerType = CMD::CenterServer::RegisterServer::eServerTypeLoginServer)
		{
			pServerInfo = CServerManager::get_mutable_instance().activateServer(
				SServerInfo::eServerTypeLogin,
				nConnID);
		}

		CMD::CenterServer::RegisterServerResult registerServerResult;
		registerServerResult.bSuccess = (pServerInfo != nullptr);
		registerServerResult.nServerID = pServerInfo->m_nServerID;

		CNetPacket packet;

		packet.getMsgHeader().unMainCmd = (uint16_t)CMD::EMainCmd::eMessageCenterServer;
		packet.getMsgHeader().unSubCmd = (uint16_t)CMD::ESubCenterCmd::eMessageRegisterServerResult;
		packet.getBody().appendBinary(&registerServerResult, sizeof(registerServerResult));

		m_TCPManager.sendData(nConnID, &packet);


		if (pServerInfo->m_eServerType == SServerInfo::eServerTypeLogin)
		{
			sendGateServerList(nConnID);	
		}
	}
	catch (const std::exception& e)
	{
		return;
	}
}

void CCenterServer::registerServerMessage(uint16_t unMainCmd, uint16_t unSubCmd, ServerMessageCallBacker callBack)
{
	int32_t nMessageID = int32_t(unMainCmd << 16) | unSubCmd;

	auto iter = m_mapServerMessageCallBack.find(nMessageID);
	if (iter == m_mapServerMessageCallBack.end())
	{
		std::list<ServerMessageCallBacker> listMessageCallBack;
		listMessageCallBack.push_back(callBack);
		m_mapServerMessageCallBack[nMessageID] = listMessageCallBack;
		return;
	}
	iter->second.push_back(callBack);
}

void CCenterServer::sendGateServerList(int nConnID)
{
	CMD::CenterServer::GateServerInfo gateServerInfo;
	CNetPacket packet;

	for (auto pGateServerInfo : CServerManager::get_mutable_instance().getAllGateServerInfo())
	{
		packet.reset();

		strcpy(gateServerInfo.szServerAddress, pGateServerInfo->m_szServerAddress.c_str());
		gateServerInfo.n32ServerID = pGateServerInfo->m_nServerID;
		gateServerInfo.n32Port = pGateServerInfo->m_nPort;

		packet.getMsgHeader().unMainCmd = (uint16_t)CMD::EMainCmd::eMessageCenterServer;
		packet.getMsgHeader().unSubCmd = (uint16_t)CMD::ESubCenterCmd::eMessageGateServerList;

		packet.getBody() << gateServerInfo;

		m_TCPManager.sendData(nConnID, &packet);
	}
}

void CCenterServer::broadcastMessageToLoginServer(CNetPacket* pNetPacket)
{
	for (auto pServerInfo : CServerManager::get_mutable_instance().getAllLoginServerInfo())
	{
		if (pServerInfo->m_eServerType == SServerInfo::eServerTypeLogin)
		{
			m_TCPManager.sendData(pServerInfo->m_nConnID, pNetPacket);
		}
	}
}

void CCenterServer::run()
{
	m_EventDispatcher.run();
}
