
#include "MemoryStream.h"
#include <cstring>

CMemoryStream::CMemoryStream()
	:writePos_(0),
	readPos_(0),
	startPos_(0)
{

}

CMemoryStream::CMemoryStream(const void* src, size_t size)
	:writePos_(0),
	readPos_(0),
	startPos_(0)
{
	appendBinary(src, size);
}

CMemoryStream::CMemoryStream(const std::string& data)
	:writePos_(0),
	readPos_(0),
	startPos_(0)
{
	appendBinary(data.c_str(), data.length());
}

CMemoryStream::~CMemoryStream()
{

}

void CMemoryStream::clear(bool clearData)
{
	startPos_ = readPos_ = writePos_ = 0;
	if (clearData)
	{
		data_.clear();
	}
}


bool CMemoryStream::empty() const
{
	return  startPos_ == writePos_;
}

void CMemoryStream::resetReadPos()
{
	readPos_ = startPos_;
}

std::string CMemoryStream::readBinary(size_t sz)
{
	if (rPos() + sz > wPos())
	{
		throw MemoryStreamException("Not enough data");
	}
	
	std::string out;
	out.resize(sz);

	memcpy(&out[0], &data_[rPos()], sz);
	readPos_ += sz;

	return out;
}

void CMemoryStream::readBinary(void* buffer, size_t sz, size_t bufferSize)
{
	if (rPos() + sz > wPos())
	{
		throw MemoryStreamException("Not enough data");
	}
	memcpy(buffer, &data_[rPos()], min(sz, bufferSize));
	readPos_ += sz;
}

std::string CMemoryStream::readString()
{
	int8_t c;
	std::string out;
	while ((c = read <int8_t>()) != '\0');
	{	
		out.append((const char*)c);
	}
	return out;
}


std::string CMemoryStream::readAll()
{
	std::string out((char*)&data_[readPos_], writePos_);
	readPos_ = writePos_;
	return out;
}

void CMemoryStream::appendBinary(const void* src, size_t sz)
{
	if (src && sz > 0)
	{
		resetReadPos();

		if (data_.size() < writePos_ + sz)
			data_.resize(writePos_ + sz);

		memcpy(&data_[writePos_], src, sz);

		writePos_ += sz;
	}
}


void CMemoryStream::appendString(const std::string& str)
{
	appendBinary(str.c_str(), str.length() + 1);
}

const void* CMemoryStream::data() const
{
	return &data_[startPos_];
}

size_t CMemoryStream::size() const
{
	return wPos() - startPos_;	
}

size_t CMemoryStream::capacity() const
{
	return data_.capacity();
}

size_t CMemoryStream::wPos() const
{
	return writePos_;
}

size_t CMemoryStream::rPos() const
{
	return readPos_;
}

size_t CMemoryStream::readSize() const
{
	return readPos_ - startPos_;
}

void CMemoryStream::remove(size_t offset, size_t count)
{
	if (startPos_ + offset + count > wPos())
	{
		throw MemoryStreamException("out of range ");
	}
		
	memmove(&data_[startPos_ + offset], &data_[startPos_+ offset + count], writePos_ - (startPos_ + offset + count));

	readPos_ = startPos_;
	writePos_ -= count;
}

void CMemoryStream::removeFront(size_t count)
{
	if (startPos_ + count > wPos())
	{
		throw MemoryStreamException("out of range ");
	}

	startPos_ += count;

	if (startPos_ == writePos_)
	{
		startPos_ = 0;
		writePos_ = 0;
	}

	readPos_ = startPos_;

}
