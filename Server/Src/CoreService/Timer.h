#pragma once

#include <queue>
#include <chrono>
#include <functional>

class CTimerItem;
class CTimerManager;

using TimeoutCallBack = std::function<void(CTimerItem*)>;


class CTimerItem
{
public:

	CTimerItem(CTimerManager& timerManager, TimeoutCallBack callBack,time_t elapseTime);
	~CTimerItem();

public:

	time_t remainTime() const;

	bool isTimeout() const;

	void triggerTimeout();

	void cancelTimer();

	bool isCancel();
private:
	std::chrono::steady_clock::time_point m_startTimePoint;
	CTimerManager& m_timerManager;

	TimeoutCallBack m_timeoutCallBack;
	time_t m_elapseTime;

	bool m_bCancel;
};

class CTimeItemCompare
{
public:
	bool operator() (const CTimerItem* a, const CTimerItem* b)
	{
		return a->remainTime() > b->remainTime();
	}
};

class CTimerManager
{
public:
	CTimerManager();
	~CTimerManager();

public:
	CTimerItem* addTimer(time_t elapseTime, TimeoutCallBack callBack);
	void poll();

private:
	std::priority_queue<CTimerItem*, std::vector<CTimerItem*>, decltype(CTimeItemCompare())> m_queTimerItem;
};

