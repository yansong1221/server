#pragma once

#include <string>

class TCPListener
{
public:
	/**
	* 设置监听地址和端口
	* @address 监听ipv4地址
	* @port    监听端口
	*/
	virtual void setListenAddress(const std::string& address, int port) = 0;
};

