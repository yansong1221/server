#pragma once

#include <string>

class TCPListener
{
public:
	/**
	* ���ü�����ַ�Ͷ˿�
	* @address ����ipv4��ַ
	* @port    �����˿�
	*/
	virtual void setListenAddress(const std::string& address, int port) = 0;
};

