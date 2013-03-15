/*
 *        Name: ByteBuffer.h
 *
 *  Created on: 2011-2-22
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_BYTEBUFFER_H__
#define __MXCORE_BYTEBUFFER_H__

#include <stdint.h>
#include <mxcore/Assert.h>
#include <mxcore/ByteOrder.h>

typedef unsigned char BYTE;

namespace mxcore
{

class ByteBuffer
{
public:
	ByteBuffer(uint32_t capacity);
	ByteBuffer(void* buffer, uint32_t len, uint32_t offset = 0,
			bool bManager = false);
	virtual ~ByteBuffer(void);

	uint32_t remaining(void) const;
	ByteBuffer& clear(void);

	ByteBuffer& put(BYTE b);
	ByteBuffer& put(int16_t value);
	ByteBuffer& put(uint16_t value);
	ByteBuffer& put(int32_t value);
	ByteBuffer& put(uint32_t value);
	ByteBuffer& put(int64_t value);
	ByteBuffer& put(uint64_t value);

	ByteBuffer& put(uint32_t index, BYTE b);
	ByteBuffer& put(const void* data, uint32_t len);
	ByteBuffer& put(ByteBuffer& src);
	ByteBuffer& put(ByteBuffer& src, uint32_t len);

	BYTE& operator [](uint32_t index);
	BYTE operator [](uint32_t index) const;

	BYTE get(void);
	BYTE get(uint32_t index);
	ByteBuffer& get(void* dst, uint32_t len);
	ByteBuffer& get(ByteBuffer& dst, uint32_t len);
	ByteBuffer& get(ByteBuffer& dst);

	int16_t getShort(void);
	uint16_t getUShrot(void);
	int32_t getInt(void);
	uint32_t getUInt(void);
	int64_t getLong(void);
	uint64_t getULong(void);

	BYTE* getBuffer(void) const;
	BYTE* getOrgBuffer(void) const;
	ByteBuffer& flip(void);

	uint32_t getPosition(void) const;
	ByteBuffer& setPosition(uint32_t pos);

	uint32_t getLimit(void) const;
	ByteBuffer& setLimit(uint32_t limit);

	uint32_t getCapacity(void) const;

	void setByteOrder(const ByteOrder& order);
	const ByteOrder& getByteOrder(void) const;
private:
	ByteBuffer& putImpl(const void* buf, uint32_t len);
	ByteBuffer& getImpl(void* buf, uint32_t len);
private:
	uint32_t capacity_;
	uint32_t position_;
	uint32_t limit_;

	bool bManager_;
	BYTE* buffer_;

	ByteOrder byteOrder_;
};

inline uint32_t ByteBuffer::remaining(void) const
{
	return limit_ - position_;
}

inline ByteBuffer& ByteBuffer::clear(void)
{
	position_ = 0;
	limit_ = capacity_;
	return *this;
}

inline BYTE* ByteBuffer::getBuffer(void) const
{
	MX_ASSERT(remaining() > 0);

	return buffer_ + position_;
}

inline BYTE* ByteBuffer::getOrgBuffer(void) const
{
	return buffer_;
}

inline ByteBuffer& ByteBuffer::flip(void)
{
	limit_ = position_;
	position_ = 0;
	return *this;
}

inline uint32_t ByteBuffer::getPosition(void) const
{
	return position_;
}

inline ByteBuffer& ByteBuffer::setPosition(uint32_t pos)
{
	MX_ASSERT(pos <= limit_);

	position_ = pos;
	return *this;
}

inline uint32_t ByteBuffer::getLimit(void) const
{
	return limit_;
}

inline ByteBuffer& ByteBuffer::setLimit(uint32_t limit)
{
	MX_ASSERT(limit <= capacity_);

	limit_ = limit;

	if (position_ > limit_)
	{
		position_ = limit_;
	}

	return *this;
}

inline uint32_t ByteBuffer::getCapacity(void) const
{
	return capacity_;
}

inline void ByteBuffer::setByteOrder(const ByteOrder& order)
{
	byteOrder_ = order;
}

inline const ByteOrder& ByteBuffer::getByteOrder(void) const
{
	return byteOrder_;
}

} // namespace mxcore

#endif /* __MXCORE_BYTEBUFFER_H__ */
