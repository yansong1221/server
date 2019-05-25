#pragma once

#include <memory>
#include <boost/asio.hpp>
#include "MemoryStream.h"

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

	void startRecv();

	void sendData(const void* pData, int nSize);

	void setConnectOk();
	bool isConnectOk() const;
private:
	void clear();
private:
	boost::asio::ip::tcp::socket m_pSocket;

	int m_nConnID;
	bool m_bConnected;

	std::string m_tempReadBuffer;

	CMemoryStream m_readBuffer;

	CTCPSeesionManager& m_TCPSeesionManager;
	//发送数据缓冲区
	std::string m_sendBuffer,m_senddingBuffer;
	//最后接收的时间点
	std::chrono::steady_clock::time_point m_LastRecvTimePoint;
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
	static uint16_t s_bindIndex;
	CTCPManager& m_TCPManager;

	int m_nNextArrayIndex;
};

