#include "GateServer.h"
#include "../CoreService/LogService.h"
#include "../MessageDefine/MessageCmd.h"
#include "GateServerConfig.h"
#include "fmt/core.h"
#include "LoginServerManager.h"

CGateServer::CGateServer()
	:m_tcpManager(m_eventDispatcher,*this),
	m_pCenterServerSeesion(nullptr)
{

}

CGateServer::~CGateServer()
{

}

void CGateServer::onNetworkMessage(int nConnID, CNetPacket* pNetPacket)
{
	//中心服务器消息
	if (m_pCenterServerSeesion && 
		m_pCenterServerSeesion->getConnID() == nConnID)
	{
		return onMeesageFromCenterServer(pNetPacket);
	}

	//网关直接处理消息
	if (onMessageGateServer(nConnID, pNetPacket))
	{
		return;
	}

}

void CGateServer::onConnected(int nConnID)
{

}

void CGateServer::onDisconnected(int nConnID)
{
	//中心服务器断开连接
	if (m_pCenterServerSeesion && 
		m_pCenterServerSeesion->getConnID() == nConnID)
	{
		reconnectCenterServer();
		return;
	}

	//登陆服务器断开连接
	auto pLoginServer = CLoginServerManager::get_mutable_instance().findLoginServerByConnID(nConnID);
	if (pLoginServer)
	{

		CLogService::get_mutable_instance().logWarning(fmt::format("登陆服务器断开连接,ConnID:{},ServerID:{}",
			pLoginServer->m_nConnID,
			pLoginServer->m_nServerID));

		CLoginServerManager::get_mutable_instance().serverDisconnect(nConnID);

		return;
	}
	
}

void CGateServer::reconnectCenterServer()
{
	if (m_pCenterServerSeesion && m_pCenterServerSeesion->isConnectOk())
	{
		return;
	}

	CLogService::get_mutable_instance().logInfo("正在尝试重新连接中心服务器...");
	m_tcpManager.asyncConnectTo(
		CGateServerConfig::get_mutable_instance().getConfigInfo().m_centerServerAddress.m_szServerAddress,
		CGateServerConfig::get_mutable_instance().getConfigInfo().m_centerServerAddress.m_nServerPort,
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

void CGateServer::connectCenterServer()
{
	// 连接中心服务器
	while (m_pCenterServerSeesion == nullptr)
	{
		CLogService::get_mutable_instance().logInfo(
			fmt::format("正在连接中心服务器[{}:{}]...",
				CGateServerConfig::get_mutable_instance().getConfigInfo().m_centerServerAddress.m_szServerAddress,
				CGateServerConfig::get_mutable_instance().getConfigInfo().m_centerServerAddress.m_nServerPort)
		);

		m_pCenterServerSeesion = m_tcpManager.connectTo(
			CGateServerConfig::get_mutable_instance().getConfigInfo().m_centerServerAddress.m_szServerAddress,
			CGateServerConfig::get_mutable_instance().getConfigInfo().m_centerServerAddress.m_nServerPort
		);

		//等待2秒
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
	CLogService::get_mutable_instance().logInfo("中心服务器连接成功！");
	registerServerToCenterServer();
}

void CGateServer::registerServerToCenterServer()
{
	CMD::CenterServer::RegisterServer RegisterServer;
	RegisterServer.eServerType = CMD::CenterServer::RegisterServer::eServerTypeGateServer;
	strcpy(RegisterServer.szServerAddress, m_tcpManager.getListenAddress().c_str());
	RegisterServer.n32Port = m_tcpManager.getListenPort();

	CNetPacket packet;
	packet.getMsgHeader().unMainCmd = (uint16_t)CMD::EMainCmd::eMessageCenterServer;
	packet.getMsgHeader().unSubCmd = (uint16_t)CMD::ESubCenterCmd::eMessageRegisterServer;

	CMemoryStream& body = packet.getBody();
	body.appendBinary(&RegisterServer, sizeof(RegisterServer));

	sendMessageToCenterServer(&packet);
}

void CGateServer::onMeesageFromCenterServer(CNetPacket* pNetPacket)
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

bool CGateServer::onMessageGateServer(int nConnID, CNetPacket* pNetPacket)
{
	SMsgHeader& msgHeader = pNetPacket->getMsgHeader();
	int32_t nMessageID = int32_t(msgHeader.unMainCmd << 16) | msgHeader.unSubCmd;

	auto iter = m_mapGateServerMessageCallBack.find(nMessageID);
	if (iter == m_mapGateServerMessageCallBack.end())
	{
		assert(false);
		return false;
	}
	for (auto callBack : iter->second)
	{
		callBack(nConnID,pNetPacket);
	}
	return true;
}

void CGateServer::registerCenterServerMessage(uint16_t nMainCmd, uint16_t nSubCmd, CenterServerMessageCallBacker callBack)
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

void CGateServer::registerGateServerMessage(uint16_t nMainCmd, uint16_t nSubCmd, GateServerMessageCallBacker callBack)
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

bool CGateServer::listenLocalAddress()
{
	try
	{
		//监听本地端口
		m_tcpManager.setAddress(
			CGateServerConfig::get_mutable_instance().getConfigInfo().m_localAddress.m_szServerAddress,
			CGateServerConfig::get_mutable_instance().getConfigInfo().m_localAddress.m_nServerPort
		);

		m_tcpManager.listen();
		m_tcpManager.startAccept();
	}
	catch (const std::exception& e)
	{
		CLogService::get_mutable_instance().logInfo(fmt::format("监听本地端口[{}:{}]失败!错误消息:{}", 
			CGateServerConfig::get_mutable_instance().getConfigInfo().m_localAddress.m_szServerAddress,
			CGateServerConfig::get_mutable_instance().getConfigInfo().m_localAddress.m_nServerPort, 
			e.what()));
		return false;
	}

	return true;
}

void CGateServer::registerAllMessage()
{
	//中心服务器消息注册
	registerCenterServerMessage((uint16_t)CMD::EMainCmd::eMessageCenterServer, (uint16_t)CMD::ESubCenterCmd::eMessageRegisterServerResult, std::bind(&CGateServer::onCenterServerRegisterResult, this, std::placeholders::_1));

	//网关直接处理消息注册
	registerGateServerMessage((uint16_t)CMD::EMainCmd::eMessageGateServer, (uint16_t)CMD::ESubGateCmd::eMessageRegisterServer, std::bind(&CGateServer::onGateServerRegisterServer, this, std::placeholders::_1, std::placeholders::_2));
}

void CGateServer::sendMessageToCenterServer(CNetPacket* pNetPacket)
{
	if (m_pCenterServerSeesion)
	{
		m_tcpManager.sendData(m_pCenterServerSeesion->getConnID(), pNetPacket);
	}
}

void CGateServer::onCenterServerRegisterResult(CNetPacket* pNetPacket)
{
	try
	{
		CMD::CenterServer::RegisterServerResult registerServerResult;
		pNetPacket->getBody() >> registerServerResult;

		if (registerServerResult.bSuccess)
		{
			CLogService::get_mutable_instance().logInfo("注册中心服务器成功!");
		}
		else
		{
			CLogService::get_mutable_instance().logInfo("注册中心服务器失败!");
		}
	}
	catch (const std::exception&)
	{

	}
}

void CGateServer::onGateServerRegisterServer(int nConnID, CNetPacket* pNetPacket)
{
	try
	{
		CMD::GateServer::RegisterServer registerServer;
		pNetPacket->getBody() >> registerServer;

		auto pLoginServer = CLoginServerManager::get_mutable_instance().addLoginServerInfo(registerServer.nServerID, nConnID);

		CMD::GateServer::RegisterServerResult registerServerResult;
		registerServerResult.bSuccess = (pLoginServer != nullptr);

		CNetPacket packet;

		packet.getMsgHeader().unMainCmd = (uint16_t)CMD::EMainCmd::eMessageGateServer;
		packet.getMsgHeader().unSubCmd = (uint16_t)CMD::ESubGateCmd::eMessageRegisterServerResult;

		packet.getBody() << registerServerResult;

		m_tcpManager.sendData(nConnID, &packet);

	}
	catch (const std::exception&)
	{

	}
}

bool CGateServer::startUp()
{
	CLogService::get_mutable_instance().logInfo("正在启动服务器...");

	if (CGateServerConfig::get_mutable_instance().loadConfig() == false)
	{
		CLogService::get_mutable_instance().logFatal("载入服务器配置失败！");
		return false;
	}

	if (listenLocalAddress() == false)
	{
		return false;
	}
	registerAllMessage();
	connectCenterServer();
	
	CLogService::get_mutable_instance().logInfo("服务器启动成功！");
	return true;
}

void CGateServer::run()
{
	m_eventDispatcher.run();
}

