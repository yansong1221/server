#pragma once

#include <thread>
#include <condition_variable>

class Thread
{
public:
	Thread();
	virtual ~Thread();
public:
	/**
	* ���߳�����ǰ���ã��������false�߳̾Ͳ�����
	*/
	virtual bool onThreadStart() = 0;
	/**
	* ���߳̽���ʱ���ã���ɺ��߳�����
	*/
	virtual	void onThreadEnd() = 0;
	/**
	* �߳�ִ��������
	*/
	virtual bool onThreadRun() = 0;
public:
	/**
	* �����߳�
	*/
	bool startThead();
	/**
	* ֹͣ�̣߳����������ȴ�����
	*/
	virtual bool stopThread();
	/**
	* �鿴��ǰ�߳��Ƿ�������
	*/
	bool isRuning() const;

private:
	/**
	* �ڲ��߳�ִ�к���
	*/
	void threradRun();
private:
	volatile bool runStatus_;

	std::thread thread_;
	std::condition_variable cv_;
	std::mutex	mt_;
};

