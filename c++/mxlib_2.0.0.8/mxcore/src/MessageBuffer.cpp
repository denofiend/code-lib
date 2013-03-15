/*
 *        Name: MessageBuffer.cpp
 *
 *  Created on: 2011-2-15
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <string.h>
#include <mxcore/Number.h>
#include <mxcore/MessageBuffer.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

MessageBuffer::MessageBuffer(uint32_t capacity) :
	buffer_(NULL), capacity_(capacity + 1), readPos_(0), writePos_(0),
			byteOrder_(ByteOrder::NATIVE_ORDER)
{
	buffer_ = new char[capacity_];
}

MessageBuffer::~MessageBuffer(void)
{
	delete[] buffer_;
	buffer_ = NULL;
}

MessageBuffer& MessageBuffer::put(char ch)
{
	MX_ASSERT(writable() > 0);

	buffer_[writePos_++] = ch;

	if (writePos_ >= capacity_)
	{
		writePos_ -= capacity_;
	}
	return *this;
}

MessageBuffer& MessageBuffer::put(int16_t value)
{
	MX_ASSERT(writable() >= sizeof(value));

	int16_t orderValue = ByteOrder::NATIVE_ORDER.toOrder(value, byteOrder_);
	return putImpl(&orderValue, sizeof(orderValue));
}

MessageBuffer& MessageBuffer::put(uint16_t value)
{
	MX_ASSERT(writable() >= sizeof(value));

	uint16_t orderValue = ByteOrder::NATIVE_ORDER.toOrder(value, byteOrder_);
	return putImpl(&orderValue, sizeof(orderValue));
}

MessageBuffer& MessageBuffer::put(int32_t value)
{
	MX_ASSERT(writable() >= sizeof(value));

	int32_t orderValue = ByteOrder::NATIVE_ORDER.toOrder(value, byteOrder_);
	return putImpl(&orderValue, sizeof(orderValue));
}

MessageBuffer& MessageBuffer::put(uint32_t value)
{
	MX_ASSERT(writable() >= sizeof(value));

	uint32_t orderValue = ByteOrder::NATIVE_ORDER.toOrder(value, byteOrder_);
	return putImpl(&orderValue, sizeof(orderValue));
}

MessageBuffer& MessageBuffer::put(int64_t value)
{
	MX_ASSERT(writable() >= sizeof(value));

	int64_t orderValue = ByteOrder::NATIVE_ORDER.toOrder(value, byteOrder_);
	return putImpl(&orderValue, sizeof(orderValue));
}

MessageBuffer& MessageBuffer::put(uint64_t value)
{
	MX_ASSERT(writable() >= sizeof(value));

	uint64_t orderValue = ByteOrder::NATIVE_ORDER.toOrder(value, byteOrder_);
	return putImpl(&orderValue, sizeof(orderValue));
}

MessageBuffer& MessageBuffer::put(const void* data, uint32_t len)
{
	MX_ASSERT(NULL != data && len > 0);
	MX_ASSERT(len <= writable());

	return putImpl(data, len);
}

char MessageBuffer::getByte(void)
{
	MX_ASSERT(readable() > 0);

	char ch = buffer_[readPos_++];

	if (readPos_ >= capacity_)
	{
		readPos_ = 0;
	}
	return ch;
}

int16_t MessageBuffer::getShort(void)
{
	int16_t ret;
	MX_ASSERT(readable() >= sizeof(int16_t));

	getImpl(&ret, sizeof(int16_t));
	return byteOrder_.toOrder(ret, ByteOrder::NATIVE_ORDER);
}

uint16_t MessageBuffer::getUShrot(void)
{
	uint16_t ret;
	MX_ASSERT(readable() >= sizeof(ret));

	getImpl(&ret, sizeof(uint16_t));
	return byteOrder_.toOrder(ret, ByteOrder::NATIVE_ORDER);
}

int32_t MessageBuffer::getInt(void)
{
	int32_t ret;
	MX_ASSERT(readable() >= sizeof(ret));

	getImpl(&ret, sizeof(int32_t));
	return byteOrder_.toOrder(ret, ByteOrder::NATIVE_ORDER);
}

uint32_t MessageBuffer::getUInt(void)
{
	uint32_t ret;
	MX_ASSERT(readable() >= sizeof(ret));

	getImpl(&ret, sizeof(uint32_t));
	return byteOrder_.toOrder(ret, ByteOrder::NATIVE_ORDER);
}

int64_t MessageBuffer::getLong(void)
{
	int64_t ret;
	MX_ASSERT(readable() >= sizeof(ret));

	getImpl(&ret, sizeof(int64_t));
	return byteOrder_.toOrder(ret, ByteOrder::NATIVE_ORDER);
}

uint64_t MessageBuffer::getULong(void)
{
	uint64_t ret;
	MX_ASSERT(readable() >= sizeof(ret));

	getImpl(&ret, sizeof(uint64_t));
	return byteOrder_.toOrder(ret, ByteOrder::NATIVE_ORDER);
}

MessageBuffer& MessageBuffer::get(uint32_t len)
{
	MX_ASSERT(len <= readable());

	readPos_ += len;

	if (readPos_ >= capacity_)
	{
		readPos_ -= capacity_;
	}

	return *this;
}

MessageBuffer& MessageBuffer::add(uint32_t len)
{
	MX_ASSERT(len <= writable());

	writePos_ += len;

	if (writePos_ >= capacity_)
	{
		writePos_ -= capacity_;
	}

	return *this;
}

uint32_t MessageBuffer::fullForPut(mxos::IOVEC* vec, uint32_t count) const
{
	MX_ASSERT(NULL != vec && count > 0);

	int wable = writable();

	MX_ASSERT(wable > 0);

	int len = capacity_ - writePos_;

	if (len > wable)
	{
		len = wable;
	}

	vec[0].iov_base = buffer_ + writePos_;
	vec[0].iov_len = len;

	wable -= len;

	if (wable > 0 && count > 1)
	{
		vec[1].iov_base = buffer_;
		vec[1].iov_len = wable;
		return 2;
	}

	return 1;
}

uint32_t MessageBuffer::fullForGet(mxos::IOVEC* vec, uint32_t count) const
{
	MX_ASSERT(NULL != vec && count > 0);
	int rable = readable();
	MX_ASSERT(rable > 0);

	int len = capacity_ - readPos_;

	if (len > rable)
	{
		len = rable;
	}

	vec[0].iov_base = buffer_ + readPos_;
	vec[0].iov_len = len;

	rable -= len;

	if (rable > 0 && count > 1)
	{
		vec[1].iov_base = buffer_;
		vec[1].iov_len = rable;
		return 2;
	}

	return 1;
}

MessageBuffer& MessageBuffer::putImpl(const void* data, uint32_t len)
{
	const char* p = (const char*) data;

	if (readPos_ > writePos_ || (len < capacity_ - writePos_))
	{
		::memcpy(buffer_ + readPos_, p, len);
		writePos_ += len;
	}
	else
	{
		::memcpy(buffer_ + writePos_, p, (capacity_ - writePos_));
		::memcpy(buffer_, p + (capacity_ - writePos_),
				len - (capacity_ - writePos_));
		writePos_ = len - (capacity_ - writePos_);
	}

	return *this;
}

MessageBuffer& MessageBuffer::getImpl(void* data, uint32_t len)
{
	char* p = (char*) data;

	if (readPos_ < writePos_ || (len <= capacity_ - readPos_))
	{
		::memcpy(p, buffer_ + readPos_, len);
		readPos_ += len;
	}
	else
	{
		::memcpy(p, buffer_ + readPos_, (capacity_ - readPos_));
		::memcpy(p + (capacity_ - readPos_), buffer_,
				len - (capacity_ - readPos_));
		readPos_ = len - (capacity_ - readPos_);
	}

	return *this;
}

} // namespace mxcore
