#pragma once

#include "Platform.h"
#include "../Common/Thread.h"
#include "Connection.h"

class NetPacket;
class IEventPoller;

class IEventListenerDelegate
{
public:
	/**
	* �����µ����ӻص�
	*/
	virtual void onListenerNewConnect(uint32_t connID) = 0;
	/**
	* ���µ�������Ϣ��
	*/
	virtual void onListenerMessage(uint32_t connID, NetPacket* packet) = 0;
	/**
	* �����ӹر�
	*/
	virtual void onListenerClose(uint32_t connID) = 0;
};

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
	void setEventListenerDelegate(IEventListenerDelegate* eventListenerDelegate);

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
	IEventListenerDelegate* eventListenerDelegate_;
	IEventPoller* eventPoller_;

	ConnectionManager TCPConnectionManager_;

};

