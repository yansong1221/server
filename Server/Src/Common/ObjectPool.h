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
			objects_.push_back(new T());
		}
	}
	~ObjectPool();

	T* createObject()
	{
		if (!objects_.empty())
		{
			T* obj = objects_.front();
			objects_.pop_front();
			return obj;
		}
		T* obj = new T();
		return obj;
	}
	void reclaimObject(T* obj)
	{
		objects_.push_back(obj);
	}

	size_t size() const
	{
		return objects_.size();
	}
private:
	OBJECTS objects_;
};

