#include "MemoryStream.h"
#include <cstring>

CMemoryStream::CMemoryStream()
	:writePos_(0),
	readPos_(0)
{

}

CMemoryStream::CMemoryStream(const void* src, size_t size)
	:writePos_(0),
	readPos_(0)
{
	appendBinary(src, size);
}

CMemoryStream::CMemoryStream(const std::string& data)
	:writePos_(0),
	readPos_(0)
{
	appendBinary(data.c_str(), data.length());
}

CMemoryStream::~CMemoryStream()
{

}

void CMemoryStream::clear(bool clearData)
{
	readPos_ = writePos_ = 0;
	if (clearData) data_.clear();
}


bool CMemoryStream::empty() const
{
	return wPos() == 0;
}

void CMemoryStream::resetReadPos()
{
	readPos_ = 0;
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
	resetReadPos();

	if (data_.size() < writePos_ + sz)
		data_.resize(writePos_ + sz);

	memcpy(&data_[writePos_], src, sz);

	writePos_ += sz;
}


void CMemoryStream::appendString(const std::string& str)
{
	appendBinary(str.c_str(), str.length() + 1);
}

const void* CMemoryStream::data() const
{
	return data_.data();
}

size_t CMemoryStream::size() const
{
	return wPos();
}

size_t CMemoryStream::wPos() const
{
	return writePos_;
}

size_t CMemoryStream::rPos() const
{
	return readPos_;
}

void CMemoryStream::remove(size_t offset, size_t count)
{
	if (offset + count > wPos())
		throw MemoryStreamException("out of range ");

	if (offset + count < writePos_)
		memmove(&data_[offset], &data_[offset + count], writePos_ - (offset + count));

	readPos_ = 0;
	writePos_ -= count;
}
