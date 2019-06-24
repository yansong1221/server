#pragma once

#include "Platform.h"
#include "../Common/Thread.h"
#include "Connection.h"
#include "EventPoller.h"

class NetPacket;

using TCPListenerCloseHandler = std::function<void(uint32_t)>;
using TCPListenerMessageHandler = std::function<void(uint32_t, NetPacket*)>;
using TCPListenerNewConnectHandler = std::function<void(uint32_t)>;

class TCPListener : public Thread
{
public:

	TCPListener();
	~TCPListener();
	/**
	* ���ü�����ַ�Ͷ˿�
	* @address ����ipv4��ַ
	* @port    �����˿�
	*/
	void listenAddress(const std::string& address, int port);

	/**
	* ���������¼��ӿ�
	*/
	void setCloseHandler(TCPListenerCloseHandler handler);
	void setMessageHandler(TCPListenerMessageHandler handler);
	void setNewConnectHandler(TCPListenerNewConnectHandler handler);

	bool closeConnection(Connection* conn);

	Connection* findConnection(uint32_t connID);
protected:
	/**
	* ���߳�����ǰ���ã��������false�߳̾Ͳ�����
	*/
	virtual bool onThreadStart();
	/**
	* ���߳̽���ʱ���ã���ɺ��߳�����
	*/
	virtual	void onThreadEnd();
	/**
	* �߳�ִ��������
	*/
	virtual bool onThreadRun();

private:
	void onAccept(SOCKET fd);

private:
	SOCKET fd_;

	EventPoller* eventPoller_;

	TCPListenerCloseHandler listenerCloseHandler_;
	TCPListenerMessageHandler listenerMessageHandler_;
	TCPListenerNewConnectHandler listenerNewConnectHandler_;

	ObjectPool<Connection>  connections_;
	uint16_t roundValue_ = 0;
};

