#pragma once

#include "Platform.h"

using AcceptHandler = std::function<void(SOCKET)>;
using ReadHandler = std::function<bool(size_t)>;
using WriteHandler = std::function<bool(size_t)>;

class IEventPoller
{
public:
	/**
	* ��ʼ��������
	* @fd �����ļ�������
	* @buffer ���յĻ�����
	* @sz ���յĻ�������С
	* @handler ������ɻص�֪ͨ
	*/
	virtual bool asyncRecv(SOCKET fd, void* buffer, size_t sz, ReadHandler handler) = 0;

	/**
	* ��ʼ��������
	* @fd ����������
	* @handler �����ӻص�
	*/
	virtual bool asyncAccept(SOCKET fd, AcceptHandler handler) = 0;
	/**
	* ��ʼ��������
	* @fd �����ļ�������
	* @buffer ���͵Ļ�����
	* @sz ���͵Ļ�������С
	* @handler ������ɻص�֪ͨ
	*/
	virtual bool asyncSend(SOCKET fd, void* buffer, size_t sz, WriteHandler handler) = 0;

	/**
	* ִ��poll�¼�
	*/
	virtual int update() = 0;
};

