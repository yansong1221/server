#pragma once

#include <list>

template <class T,int objectSize = 32>
class ObjectPool
{
public:
	using OBJECTS = std::list<T*>;

	ObjectPool()
	{
		for (int i = 0; i < objectSize; ++i)
		{
			freeObjects_.push_back(new T());
		}
	}
	~ObjectPool()
	{
		while (!freeObjects_.empty())
		{
			auto obj = freeObjects_.front();
			delete obj;
			freeObjects_.pop_front();
		}

		while (!activeObjects_.empty())
		{
			auto obj = activeObjects_.front();
			delete obj;
			activeObjects_.pop_front();
		}
	}

	T* createObject()
	{
		if (!freeObjects_.empty())
		{
			T* obj = freeObjects_.front();
			freeObjects_.pop_front();
			activeObjects_.push_back(obj);
			return obj;
		}
		T* obj = new T();
		activeObjects_.push_back(obj);
		return obj;
	}
	bool reclaimObject(T* obj)
	{
		auto iter = std::find(activeObjects_.begin(), activeObjects_.end(), obj);
		if (iter == activeObjects_.end())
		{
			return false;
		}
			
		activeObjects_.erase(iter);
		freeObjects_.push_back(obj);

		return true;
	}

	size_t freeSize() const
	{
		return freeObjects_.size();
	}

	size_t activeSize() const
	{
		return activeObjects_.size();
	}

	bool hasObject(T* obj) const
	{
		auto iter = std::find(activeObjects_.begin(), activeObjects_.end(), obj);
		if (iter == activeObjects_.end())
		{
			return false;
		}
		return true;
	}

	const OBJECTS& getActiveObjects() const
	{
		return activeObjects_;
	}
private:
	OBJECTS freeObjects_;
	OBJECTS activeObjects_;
};

