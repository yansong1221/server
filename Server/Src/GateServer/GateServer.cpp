#include "GateServer.h"
#include "../CoreService/LogService.h"
#include "../MessageDefine/MessageCmd.h"

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
	if (m_pCenterServerSeesion && m_pCenterServerSeesion->getConnID() == nConnID)
	{
		return onMeesageFromCenterServer(pNetPacket);
	}

}

void CGateServer::onConnected(int nConnID)
{

}

void CGateServer::onDisconnected(int nConnID)
{
	//中心服务器断开连接
	if (m_pCenterServerSeesion && m_pCenterServerSeesion->getConnID() == nConnID)
	{
		m_pCenterServerSeesion = nullptr;
		reconnectCenterServer();
	}
}

void CGateServer::reconnectCenterServer()
{
	if (m_pCenterServerSeesion == nullptr)
	{
		CLogService::get_mutable_instance().logInfo("正在尝试重新连接中心服务器...");
		m_tcpManager.asyncConnectTo("127.0.0.1", 8800, [this](CTCPSeesion* pTCPSeesion)
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
}

void CGateServer::registerServerToCenterServer()
{
	CMDRegisterServer RegisterServer;
	RegisterServer.eServerType = CMDRegisterServer::eServerTypeGateServer;
	strcpy(RegisterServer.szServerAddress, m_tcpManager.getListenAddress().c_str());
	RegisterServer.n32Port = m_tcpManager.getListenPort();

	CNetPacket packet;
	packet.getMsgHeader().unMainCmd = EMessageMainCmd::eMessageCenterServer;
	packet.getMsgHeader().unSubCmd = EMessageSubCenterServer::eMessageRegisterServer;

	CMemoryStream& body = packet.getBody();
	body.appendBinary(&RegisterServer, sizeof(RegisterServer));

	m_tcpManager.sendData(m_pCenterServerSeesion->getConnID(), &packet);
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

void CGateServer::registerCenterServerMsseage(uint16_t nMainCmd, uint16_t nSubCmd, std::function<void(CNetPacket*)> callBack)
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

void CGateServer::onCenterServerRegisterResult(CNetPacket* pNetPacket)
{

}

bool CGateServer::startUp()
{
	CLogService::get_mutable_instance().logInfo("正在启动服务器...");
	registerCenterServerMsseage(EMessageMainCmd::eMessageCenterServer, EMessageSubCenterServer::eMessageRegisterServerResult, std::bind(&CGateServer::onCenterServerRegisterResult, this, std::placeholders::_1));
	try
	{
		//监听本地端口
		m_tcpManager.setAddress("127.0.0.1", 8801);
		m_tcpManager.listen();
		m_tcpManager.startAccept();
	}
	catch (const std::exception& e)
	{
		CLogService::get_mutable_instance().logInfo("监听本地端口失败!");
		return false;
	}
	
	//连接中心服务器
	while (m_pCenterServerSeesion == nullptr)
	{
		CLogService::get_mutable_instance().logInfo("正在连接中心服务器...");
		m_pCenterServerSeesion = m_tcpManager.connectTo("127.0.0.1", 8800);
		//等待2秒
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
	CLogService::get_mutable_instance().logInfo("中心服务器连接成功！");
	registerServerToCenterServer();


	return true;
}

void CGateServer::run()
{
	m_eventDispatcher.run();
}

