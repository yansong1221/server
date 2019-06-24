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
	* 设置监听地址和端口
	* @address 监听ipv4地址
	* @port    监听端口
	*/
	void listenAddress(const std::string& address, int port);

	/**
	* 设置网络事件接口
	*/
	void setCloseHandler(TCPListenerCloseHandler handler);
	void setMessageHandler(TCPListenerMessageHandler handler);
	void setNewConnectHandler(TCPListenerNewConnectHandler handler);

	bool closeConnection(Connection* conn);

	Connection* findConnection(uint32_t connID);
protected:
	/**
	* 当线程启动前调用，如果返回false线程就不启动
	*/
	virtual bool onThreadStart();
	/**
	* 当线程结束时调用，完成后线程销毁
	*/
	virtual	void onThreadEnd();
	/**
	* 线程执行任务函数
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

