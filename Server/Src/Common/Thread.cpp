#include "Thread.h"
#include <cassert>


Thread::Thread()
	:runStatus_(false)
{
}


Thread::~Thread()
{
	stopThread();	
}

bool Thread::startThead()
{
	if (isRuning())
	{
		assert(false);
		return false;
	}

	thread_ = std::thread(std::bind(&Thread::threradRun, this));

	std::unique_lock<std::mutex> ul(mt_);
	cv_.wait(ul);

	return runStatus_;
}

bool Thread::stopThread()
{
	if (!isRuning())
	{
		return true;
	}

	runStatus_ = false;
	thread_.join();
	return true;
}

bool Thread::isRuning() const
{
	return runStatus_;
}

void Thread::threradRun()
{
	//等待主线程条件变量先等待
	std::this_thread::sleep_for(std::chrono::seconds(1));

	if (!this->onThreadStart())
	{
		return cv_.notify_one();
	}

	runStatus_ = true;
	cv_.notify_one();

	while (runStatus_)
	{	
		if (!this->onThreadRun())
		{
			break;
		}	
	}
	runStatus_ = false;
	return this->onThreadEnd();	
}
