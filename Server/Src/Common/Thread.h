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
	* 当线程启动前调用，如果返回false线程就不启动
	*/
	virtual bool onThreadStart() = 0;
	/**
	* 当线程结束时调用，完成后线程销毁
	*/
	virtual	void onThreadEnd() = 0;
	/**
	* 线程执行任务函数
	*/
	virtual bool onThreadRun() = 0;
public:
	/**
	* 启动线程
	*/
	bool startThead();
	/**
	* 停止线程，并且阻塞等待结束
	*/
	virtual bool stopThread();
	/**
	* 查看当前线程是否在运行
	*/
	bool isRuning() const;

private:
	/**
	* 内部线程执行函数
	*/
	void threradRun();
private:
	volatile bool runStatus_;

	std::thread thread_;
	std::condition_variable cv_;
	std::mutex	mt_;
};

