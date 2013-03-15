/*
 *        Name: ByteBuffer.cpp
 *
 *  Created on: 2011-2-22
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <string.h>
#include <mxcore/ByteBuffer.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

ByteBuffer::ByteBuffer(uint32_t capacity) :
	capacity_(capacity), position_(0), limit_(capacity_), bManager_(true),
			buffer_(NULL), byteOrder_(ByteOrder::NATIVE_ORDER)
{
	MX_ASSERT(capacity > 0);

	buffer_ = new BYTE[capacity_];
	memset(buffer_, 0, capacity_);
}

ByteBuffer::ByteBuffer(void* buffer, uint32_t len, uint32_t offset,
		bool bManager) :
	capacity_(len), position_(offset), limit_(len), bManager_(bManager),
			buffer_((BYTE*) buffer_), byteOrder_(ByteOrder::NATIVE_ORDER)
{
	MX_ASSERT(NULL != buffer && len > 0);
}

ByteBuffer::~ByteBuffer(void)
{
	if (bManager_)
	{
		delete[] buffer_;
		buffer_ = NULL;
	}
}

ByteBuffer& ByteBuffer::put(BYTE b)
{
	MX_ASSERT(remaining() > 0);

	buffer_[position_++] = b;
	return *this;
}

ByteBuffer& ByteBuffer::put(int16_t value)
{
	MX_ASSERT(remaining() >= sizeof(value));

	int16_t orderValue = ByteOrder::NATIVE_ORDER.toOrder(value, byteOrder_);
	return putImpl((const BYTE*) &orderValue, sizeof(orderValue));
}

ByteBuffer& ByteBuffer::put(uint16_t value)
{
	MX_ASSERT(remaining() >= sizeof(value));

	uint16_t orderValue = ByteOrder::NATIVE_ORDER.toOrder(value, byteOrder_);
	return putImpl((const BYTE*) &orderValue, sizeof(orderValue));
}

ByteBuffer& ByteBuffer::put(int32_t value)
{
	MX_ASSERT(remaining() >= sizeof(value));

	int32_t orderValue = ByteOrder::NATIVE_ORDER.toOrder(value, byteOrder_);
	return putImpl((const BYTE*) &orderValue, sizeof(orderValue));
}

ByteBuffer& ByteBuffer::put(uint32_t value)
{
	MX_ASSERT(remaining() >= sizeof(value));

	uint32_t orderValue = ByteOrder::NATIVE_ORDER.toOrder(value, byteOrder_);
	return putImpl((const BYTE*) &orderValue, sizeof(orderValue));
}

ByteBuffer& ByteBuffer::put(int64_t value)
{
	MX_ASSERT(remaining() >= sizeof(value));

	int64_t orderValue = ByteOrder::NATIVE_ORDER.toOrder(value, byteOrder_);
	return putImpl((const BYTE*) &orderValue, sizeof(orderValue));
}

ByteBuffer& ByteBuffer::put(uint64_t value)
{
	MX_ASSERT(remaining() > sizeof(value));

	uint64_t orderValue = ByteOrder::NATIVE_ORDER.toOrder(value, byteOrder_);
	return putImpl((const BYTE*) &orderValue, sizeof(orderValue));
}

ByteBuffer& ByteBuffer::put(uint32_t index, BYTE b)
{
	MX_ASSERT(index < limit_);

	buffer_[index] = b;
	return *this;
}

ByteBuffer& ByteBuffer::put(const void* data, uint32_t len)
{
	MX_ASSERT(NULL != data && len > 0);
	MX_ASSERT(len <= remaining());

	return putImpl(data, len);
}

ByteBuffer& ByteBuffer::put(ByteBuffer& src)
{
	uint32_t srcRem = src.remaining();
	MX_ASSERT(srcRem > 0 && srcRem < remaining());

	putImpl(src.getBuffer(), srcRem);
	src.position_ += srcRem;
	return *this;
}

ByteBuffer& ByteBuffer::put(ByteBuffer& src, uint32_t len)
{
	MX_ASSERT(len > 0);
	MX_ASSERT(len <= remaining());
	MX_ASSERT(len <= src.remaining());

	putImpl(src.getBuffer(), len);
	src.position_ += len;
	return *this;
}

BYTE& ByteBuffer::operator [](uint32_t index)
{
	MX_ASSERT(index < limit_);

	return buffer_[index];
}

BYTE ByteBuffer::operator [](uint32_t index) const
{
	MX_ASSERT(index < limit_);

	return buffer_[index];
}

BYTE ByteBuffer::get(void)
{
	MX_ASSERT(remaining() > 0);

	return buffer_[position_++];
}

BYTE ByteBuffer::get(uint32_t index)
{
	MX_ASSERT(index < limit_);

	return buffer_[index];
}

ByteBuffer& ByteBuffer::get(void* dst, uint32_t len)
{
	MX_ASSERT(NULL != dst && len > 0);
	MX_ASSERT(len <= remaining());

	return getImpl((BYTE*) dst, len);
}

ByteBuffer& ByteBuffer::get(ByteBuffer& dst, uint32_t len)
{
	MX_ASSERT(len > 0);
	MX_ASSERT(len <= remaining());
	MX_ASSERT(len <= dst.remaining());

	getImpl(dst.getBuffer(), len);
	dst.position_ += len;

	return *this;
}

ByteBuffer& ByteBuffer::get(ByteBuffer& dst)
{
	return get(dst, dst.remaining());
}

int16_t ByteBuffer::getShort(void)
{
	MX_ASSERT(remaining() >= sizeof(int16_t));

	int16_t value;
	getImpl(&value, sizeof(int16_t));
	return byteOrder_.toOrder(value, ByteOrder::NATIVE_ORDER);
}

uint16_t ByteBuffer::getUShrot(void)
{
	MX_ASSERT(remaining() >= sizeof(uint16_t));

	uint16_t value;
	getImpl(&value, sizeof(uint16_t));
	return byteOrder_.toOrder(value, ByteOrder::NATIVE_ORDER);
}

int32_t ByteBuffer::getInt(void)
{
	MX_ASSERT(remaining() >= sizeof(int32_t));

	int32_t value;
	getImpl(&value, sizeof(int32_t));
	return byteOrder_.toOrder(value, ByteOrder::NATIVE_ORDER);
}

uint32_t ByteBuffer::getUInt(void)
{
	MX_ASSERT(remaining() >= sizeof(uint32_t));

	uint32_t value;
	getImpl(&value, sizeof(uint32_t));
	return byteOrder_.toOrder(value, ByteOrder::NATIVE_ORDER);
}

int64_t ByteBuffer::getLong(void)
{
	MX_ASSERT(remaining() >= sizeof(int64_t));

	int64_t value;
	getImpl(&value, sizeof(int64_t));
	return byteOrder_.toOrder(value, ByteOrder::NATIVE_ORDER);
}

uint64_t ByteBuffer::getULong(void)
{
	MX_ASSERT(remaining() >= sizeof(uint64_t));

	uint64_t value;
	getImpl(&value, sizeof(uint64_t));
	return byteOrder_.toOrder(value, ByteOrder::NATIVE_ORDER);
}

ByteBuffer& ByteBuffer::putImpl(const void* buf, uint32_t len)
{
	::memcpy(buffer_ + position_, buf, len);
	position_ += len;

	return *this;
}

ByteBuffer& ByteBuffer::getImpl(void* buf, uint32_t len)
{
	::memcpy(buf, buffer_ + position_, len);
	position_ += len;
	return *this;
}

} // namespace mxcore
