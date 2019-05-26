
#ifndef _ENGINE_MEMORY_STREAM_H
#define	_ENGINE_MEMORY_STREAM_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <vector>
#include <stdexcept>
#include <string>

#define STREAM_ENABLE_TYPE				\
typename = typename std::enable_if<		\
std::is_same<T, uint8_t>::value ||		\
std::is_same<T, uint16_t>::value ||		\
std::is_same<T, uint32_t>::value ||		\
std::is_same<T, uint64_t>::value ||		\
std::is_same<T, int8_t>::value ||		\
std::is_same<T, int16_t>::value ||		\
std::is_same<T, int32_t>::value ||		\
std::is_same<T, int64_t>::value ||		\
std::is_class<T>::value					\
>::type


class MemoryStreamException : public std::exception 
{
public:
	MemoryStreamException(const std::string& _Message) :message_(_Message) {}
	virtual const char* what()const noexcept 
	{
		return message_.c_str();
	}
private:
	std::string message_;
};

class CMemoryStream {
public:
	CMemoryStream();
	CMemoryStream(const std::string& data);
	CMemoryStream(const void* src, size_t size);
	~CMemoryStream();

	//反序列化
	template <typename T>
	CMemoryStream& operator >> (T& value)
	{
		value = read<T>();
		return *this;
	}
	template <typename T, STREAM_ENABLE_TYPE>
	T read()
	{
		if (readPos_ + sizeof(T) > writePos_)
		{
			throw MemoryStreamException("Not enough data");
		}	

		T value = *(T*)&data_[readPos_];
		readPos_ += sizeof(T);

		return value;
	}

	std::string readBinary(size_t sz);
	std::string readString();

	//序列化
	template <typename T, STREAM_ENABLE_TYPE> CMemoryStream& operator << (const T& value)
	{
		append(value);
		return *this;
	}

	template <typename T, STREAM_ENABLE_TYPE>
	void append(const T& value) 
	{
		return appendBinary(&value, sizeof(T));
	}

	void appendBinary(const void* src, size_t size);
	void appendString(const std::string& str);

	//-----------------------------------------------------
	const void* data() const;
	size_t size() const;

	size_t wPos() const;
	size_t rPos() const;



	void clear(bool clearData = false);

	bool empty() const;

	void remove(size_t offset, size_t count);

	void resetReadPos();
	
protected:
	std::vector<uint8_t> data_;
	size_t writePos_, readPos_;
};

#endif