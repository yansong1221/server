#pragma once

#include "Platform.h"

using AcceptHandler = std::function<void(SOCKET)>;
using ReadHandler = std::function<bool(size_t)>;
using WriteHandler = std::function<bool(size_t)>;

class IEventPoller
{
public:
	/**
	* 开始接收数据
	* @fd 接收文件描述符
	* @buffer 接收的缓冲区
	* @sz 接收的缓冲区大小
	* @handler 接收完成回调通知
	*/
	virtual bool asyncRecv(SOCKET fd, void* buffer, size_t sz, ReadHandler handler) = 0;

	/**
	* 开始发送数据
	* @fd 监听描述符
	* @handler 新连接回调
	*/
	virtual bool asyncAccept(SOCKET fd, AcceptHandler handler) = 0;
	/**
	* 开始发送数据
	* @fd 发送文件描述符
	* @buffer 发送的缓冲区
	* @sz 发送的缓冲区大小
	* @handler 发送完成回调通知
	*/
	virtual bool asyncSend(SOCKET fd, void* buffer, size_t sz, WriteHandler handler) = 0;

	/**
	* 执行poll事件
	*/
	virtual int update() = 0;
};

