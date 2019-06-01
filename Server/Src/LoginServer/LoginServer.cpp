#include "LoginServer.h"
#include "../CoreService/LogService.h"
#include "fmt/core.h"
#include "../MessageDefine/MessageCmd.h"
#include "GateServerManager.h"

CLoginServer::CLoginServer()
	:m_tcpManager(m_eventDispatcher,*this),
	m_pCenterServerSeesion(nullptr),
	m_nServerID(0)
{

}

CLoginServer::~CLoginServer()
{

}

void CLoginServer::onNetworkMessage(uint32_t nConnID, CNetPacket* pNetPacket)
{
	//中心服务器消息
	if (m_pCenterServerSeesion &&
		m_pCenterServerSeesion->getConnID() == nConnID)
	{
		return onMeesageFromCenterServer(pNetPacket);
	}

	//网关服务器消息
	auto pGateServer = CGateServerManager::get_mutable_instance().findGateServerByConnID(nConnID);
	if (pGateServer == nullptr)
	{
		assert(false);
		return;
	}

	onMeesageFromGateServer(nConnID, pNetPacket);
}

void CLoginServer::onConnected(uint32_t nConnID)
{

}

void CLoginServer::onDisconnected(uint32_t nConnID)
{
	//中心服务器断开连接
	if (m_pCenterServerSeesion &&
		m_pCenterServerSeesion->getConnID() == nConnID)
	{
		//清理所有网关
		for (auto pGateServer : CGateServerManager::get_mutable_instance().getAllGateServer())
		{
			m_tcpManager.closeSeesion(pGateServer->m_nConnID);
		}
		CGateServerManager::get_mutable_instance().clear();

		m_nServerID = 0;
		reconnectCenterServer();
		return;
	}

	//网关服务器断开连接
	auto pGateServer = CGateServerManager::get_mutable_instance().findGateServerByConnID(nConnID);
	if (pGateServer == nullptr)
	{
		//assert(false);
		return;
	}

	pGateServer->m_nConnID = 0;
	pGateServer->m_eConnectStatus = CGateServer::eConnectStatusNull;
	tryConnectAllGateServer();
}

void CLoginServer::onMeesageFromCenterServer(CNetPacket* pNetPacket)
{
	SMsgHeader& msgHeader = pNetPacket->getMsgHeader();
	int32_t nMessageID = int32_t(msgHeader.unMainCmd << 16) | msgHeader.unSubCmd;

	auto iter = m_mapCenterServerMessageCallBack.find(nMessageID);
	if (iter == m_mapCenterServerMessageCallBack.end())
	{
		CLogService::get_mutable_instance().logWarning("没有注册中心服务器返回的消息ID");
		return;
	}
	for (auto callBack : iter->second)
	{
		callBack(pNetPacket);
	}
	return;

}

void CLoginServer::onMeesageFromGateServer(uint32_t nConnID, CNetPacket* pNetPacket)
{
	SMsgHeader& msgHeader = pNetPacket->getMsgHeader();
	int32_t nMessageID = int32_t(msgHeader.unMainCmd << 16) | msgHeader.unSubCmd;

	auto iter = m_mapGateServerMessageCallBack.find(nMessageID);
	if (iter == m_mapGateServerMessageCallBack.end())
	{
		return;
	}
	for (auto callBack : iter->second)
	{
		callBack(nConnID,pNetPacket);
	}
	return;

}

void CLoginServer::registerCenterServerMessage(uint16_t nMainCmd, uint16_t nSubCmd, CenterServerMessageCallBacker callBack)
{
	int32_t nMessageID = int32_t(nMainCmd << 16) | nSubCmd;

	auto iter = m_mapCenterServerMessageCallBack.find(nMessageID);
	if (iter == m_mapCenterServerMessageCallBack.end())
	{
		std::list<CenterServerMessageCallBacker> listMessageCallBack;
		listMessageCallBack.push_back(callBack);
		m_mapCenterServerMessageCallBack[nMessageID] = listMessageCallBack;
		return;
	}
	iter->second.push_back(callBack);
}

void CLoginServer::registerGateServerMessage(uint16_t nMainCmd, uint16_t nSubCmd, GateServerMessageCallBacker callBack)
{
	int32_t nMessageID = int32_t(nMainCmd << 16) | nSubCmd;

	auto iter = m_mapGateServerMessageCallBack.find(nMessageID);
	if (iter == m_mapGateServerMessageCallBack.end())
	{
		std::list<GateServerMessageCallBacker> listMessageCallBack;
		listMessageCallBack.push_back(callBack);
		m_mapGateServerMessageCallBack[nMessageID] = listMessageCallBack;
		return;
	}
	iter->second.push_back(callBack);
}

void CLoginServer::registerServerToCenterServer()
{
	CMD::CenterServer::RegisterServer RegisterServer;
	RegisterServer.eServerType = CMD::CenterServer::RegisterServer::eServerTypeLoginServer;

	CNetPacket packet;
	packet.getMsgHeader().unMainCmd = (uint16_t)CMD::EMainCmd::eMessageCenterServer;
	packet.getMsgHeader().unSubCmd = (uint16_t)CMD::ESubCenterCmd::eMessageRegisterServer;

	CMemoryStream& body = packet.getBody();
	body.appendBinary(&RegisterServer, sizeof(RegisterServer));

	m_tcpManager.sendData(m_pCenterServerSeesion->getConnID(), &packet);
}

void CLoginServer::connectCenterServer()
{
	// 连接中心服务器
	while (m_pCenterServerSeesion == nullptr)
	{
		CLogService::get_mutable_instance().logInfo(
			fmt::format("正在连接中心服务器[{}:{}]...",
				"127.0.0.1",
				8800)
		);

		m_pCenterServerSeesion = m_tcpManager.connectTo(
			"127.0.0.1",
			8800
		);

		//等待2秒
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
	CLogService::get_mutable_instance().logInfo("中心服务器连接成功！");
	registerServerToCenterServer();
}

void CLoginServer::reconnectCenterServer()
{
	if (m_pCenterServerSeesion && m_pCenterServerSeesion->isConnectOk())
	{
		return;
	}

	CLogService::get_mutable_instance().logInfo("正在尝试重新连接中心服务器...");
	m_tcpManager.asyncConnectTo(
		"127.0.0.1",
		8800,
		[this](CTCPSeesion* pTCPSeesion)
	{
		if (pTCPSeesion)
		{
			m_pCenterServerSeesion = pTCPSeesion;
			CLogService::get_mutable_instance().logInfo("中心服务器重连成功！");
			registerServerToCenterServer();
		}
		else
		{
			reconnectCenterServer();
		}
	});
}

void CLoginServer::registerAllMessage()
{
	registerCenterServerMessage((uint16_t)CMD::EMainCmd::eMessageCenterServer, (uint16_t)CMD::ESubCenterCmd::eMessageRegisterServerResult, std::bind(&CLoginServer::onCenterServerRegisterResult, this, std::placeholders::_1));
	registerCenterServerMessage((uint16_t)CMD::EMainCmd::eMessageCenterServer, (uint16_t)CMD::ESubCenterCmd::eMessageGateServerList, std::bind(&CLoginServer::OnCenterServerGateServerList, this, std::placeholders::_1));
	registerCenterServerMessage((uint16_t)CMD::EMainCmd::eMessageCenterServer, (uint16_t)CMD::ESubCenterCmd::eMessageGateOffline, std::bind(&CLoginServer::onCenterServerGateOffline, this, std::placeholders::_1));
	registerCenterServerMessage((uint16_t)CMD::EMainCmd::eMessageCenterServer, (uint16_t)CMD::ESubCenterCmd::eMessageGateServerOnline, std::bind(&CLoginServer::onCenterServerGateOnline, this, std::placeholders::_1));
	//注册网关发回的消息
	registerGateServerMessage((uint16_t)CMD::EMainCmd::eMessageGateServer, (uint16_t)CMD::ESubGateCmd::eMessageRegisterServerResult, std::bind(&CLoginServer::onGateServerRegisterResult, this, std::placeholders::_1, std::placeholders::_2));
}

void CLoginServer::tryConnectAllGateServer()
{
	auto& listGateServer =  CGateServerManager::get_mutable_instance().getAllGateServer();

	for (auto pGateServer : listGateServer)
	{
		if(pGateServer->isConnectOk()) continue;
		if(pGateServer->m_eConnectStatus != CGateServer::EConnectStatus::eConnectStatusNull) continue;

		pGateServer->m_eConnectStatus = CGateServer::EConnectStatus::eConnectStatusConnecting;

		CLogService::get_mutable_instance().logInfo(fmt::format("正在尝试连接网关服务器[{}:{}]", pGateServer->m_szAddress, pGateServer->m_nPort));
		m_tcpManager.asyncConnectTo(pGateServer->m_szAddress, pGateServer->m_nPort, [this, pGateServer](CTCPSeesion* pTCPSeesion)
		{
			bool bHasGateServer = CGateServerManager::get_mutable_instance().hasGateServer(pGateServer);

			if (pTCPSeesion)
			{
				if (bHasGateServer == false)
				{
					pTCPSeesion->close();
					return;
				}

				pGateServer->m_eConnectStatus = CGateServer::EConnectStatus::eConnectStatusSuccess;
				pGateServer->m_nConnID = pTCPSeesion->getConnID();

				CLogService::get_mutable_instance().logInfo(fmt::format("连接网关服务器[{}:{}]成功!", pGateServer->m_szAddress, pGateServer->m_nPort));

				CMD::GateServer::RegisterServer registerServer;
				registerServer.eServerType = CMD::GateServer::RegisterServer::eServerTypeLoginServer;
				registerServer.nServerID = m_nServerID;

				CNetPacket packet;
				packet.getMsgHeader().unMainCmd = (uint16_t)CMD::EMainCmd::eMessageGateServer;
				packet.getMsgHeader().unSubCmd = (uint16_t)CMD::ESubGateCmd::eMessageRegisterServer;
				packet.getBody() << registerServer;

				m_tcpManager.sendData(pGateServer->m_nConnID, &packet);
			}
			else if(bHasGateServer && pTCPSeesion == nullptr)
			{
				CLogService::get_mutable_instance().logInfo(fmt::format("连接网关服务器[{}:{}]失败!", pGateServer->m_szAddress, pGateServer->m_nPort));

				pGateServer->m_eConnectStatus = CGateServer::EConnectStatus::eConnectStatusNull;
				pGateServer->m_nConnID = 0;
				tryConnectAllGateServer();			
			}
		});
	}	
}

bool CLoginServer::startUp()
{
	CLogService::get_mutable_instance().logInfo("正在启动服务器...");

	registerAllMessage();
	connectCenterServer();

	CLogService::get_mutable_instance().logInfo("服务器启动成功！");
	return true;
}

void CLoginServer::run()
{
	m_eventDispatcher.run();
}

void CLoginServer::onCenterServerRegisterResult(CNetPacket* pNetPacket)
{
	try
	{
		CMD::CenterServer::RegisterServerResult registerServerResult;
		pNetPacket->getBody() >> registerServerResult;

		if (registerServerResult.bSuccess)
		{
			CLogService::get_mutable_instance().logInfo("注册中心服务器成功!");
			m_nServerID = registerServerResult.nServerID;
		}
		else
		{
			CLogService::get_mutable_instance().logInfo("注册中心服务器失败!");
			m_nServerID = 0;
		}
	}
	catch (const std::exception&)
	{

	}
}

void CLoginServer::OnCenterServerGateServerList(CNetPacket* pNetPacket)
{
	try
	{
		if (pNetPacket->getBody().size() % sizeof(CMD::CenterServer::GateServerInfo) != 0)
		{
			assert(false);
			return;
		}

		int nSize = pNetPacket->getBody().size() / sizeof(CMD::CenterServer::GateServerInfo);
		for (int i = 0; i < nSize; ++i)
		{
			CMD::CenterServer::GateServerInfo serverInfo;
			pNetPacket->getBody() >> serverInfo;
			CGateServerManager::get_mutable_instance().addGateServerInfo(serverInfo.szServerAddress, serverInfo.n32Port, serverInfo.n32ServerID);
		}

		//尝试连接所有GateServer
		tryConnectAllGateServer();
	}
	catch (const std::exception&)
	{

	}
}

void CLoginServer::onCenterServerGateOffline(CNetPacket* pNetPacket)
{
	try
	{
		CMD::CenterServer::GateServerOffline gateServerOffline;
		pNetPacket->getBody() >> gateServerOffline;

		auto pGateServer = CGateServerManager::get_mutable_instance().findGateServerByServerID(gateServerOffline.nServerID);
		if (pGateServer == nullptr)
		{
			assert(false);
			return;
		}

		if(pGateServer->m_nConnID != 0) m_tcpManager.closeSeesion(pGateServer->m_nConnID);
		CGateServerManager::get_mutable_instance().serverDisconnect(pGateServer->m_nConnID);
	}
	catch (const std::exception&)
	{

	}
}

void CLoginServer::onCenterServerGateOnline(CNetPacket* pNetPacket)
{
	return OnCenterServerGateServerList(pNetPacket);
}

void CLoginServer::onGateServerRegisterResult(uint32_t nConnID, CNetPacket* pNetPacket)
{
	try
	{
		CMD::GateServer::RegisterServerResult  registerServerResult;
		pNetPacket->getBody() >> registerServerResult;

		if (registerServerResult.bSuccess)
		{
			CLogService::get_mutable_instance().logInfo("注册网关服务器成功!");
		}
		else
		{
			CLogService::get_mutable_instance().logInfo("注册网关服务器失败!");
		}
	}
	catch (const std::exception&)
	{

	}
}

void CLoginServer::onGateServerRelayGameClientMessage(uint32_t nConnID, CNetPacket* pNetPacket)
{
	CMD::LoginServer::RelayHeader relayHeader;
	pNetPacket->getBody() >> relayHeader;


}
