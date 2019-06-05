#pragma once

#include <memory>
#include <boost/asio.hpp>
#include "NetPacket.h"

class CTCPSeesionManager;

class CTCPSeesion
{
public:
	CTCPSeesion(CTCPSeesionManager& TCPSeesionManager,int nConnID, int nReadBufferSize = 1024);
	~CTCPSeesion();

	boost::asio::ip::tcp::socket& getScoket();
	int getConnID() const;

	void close();

	void shutdown();

	void sendData(CNetPacket* pNetPacket = nullptr);

	void setConnectOk();
	bool isConnectOk() const;
private:
	void startRecv();
	void clear();
private:
	boost::asio::ip::tcp::socket m_pSocket;

	int m_nConnID;
	bool m_bConnected;

	std::string m_TempReadBuffer;

	CMemoryStream m_ReadBuffer;

	CTCPSeesionManager& m_TCPSeesionManager;
	//发送数据缓冲区
	std::string m_SendBuffer,m_SenddingBuffer;
	//最后接收的时间点
	std::chrono::steady_clock::time_point m_LastRecvTimePoint;
	//解析的包体
	CNetPacket m_NetPacket;
};

class ITCPEventDelegate;
class CTCPManager;
class CTCPSeesionManager
{
	friend class CTCPSeesion;
public:
	CTCPSeesionManager(uint16_t nMaxSeesionNum, CTCPManager& TCPManager);
	~CTCPSeesionManager();

	CTCPSeesion* createSeesion();

	CTCPSeesion* findSeesionByConnID(int nConnID);

	void closeSeesion(int nConnID);
	void closeSeesion(CTCPSeesion* pTCPSeesion);
private:
	ITCPEventDelegate& getTCPEventDelegate();
	boost::asio::io_service& getIOService();
private:
	std::vector<CTCPSeesion*> m_vecTCPSeesion;
	CTCPManager& m_TCPManager;
	int m_nNextArrayIndex;
};

