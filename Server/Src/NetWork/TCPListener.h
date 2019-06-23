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
	* 当有新的连接回调
	*/
	virtual void onListenerNewConnect(uint32_t connID) = 0;
	/**
	* 有新的网络消息包
	*/
	virtual void onListenerMessage(uint32_t connID, NetPacket* packet) = 0;
	/**
	* 有连接关闭
	*/
	virtual void onListenerClose(uint32_t connID) = 0;
};

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
	void setEventListenerDelegate(IEventListenerDelegate* eventListenerDelegate);

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
	IEventListenerDelegate* eventListenerDelegate_;
	IEventPoller* eventPoller_;

	ConnectionManager TCPConnectionManager_;

};

