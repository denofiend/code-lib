/*
 *        Name: MessageBuffer.h
 *
 *  Created on: 2011-2-15
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_MESSAGEBUFFER_H__
#define __MXCORE_MESSAGEBUFFER_H__

#include <stdint.h>
#include <mxcore/SocketOps.h>
#include <mxcore/SharedPtr.h>
#include <mxcore/ByteOrder.h>

namespace mxcore
{

class MessageBuffer
{
public:
	MessageBuffer(uint32_t capacity);
	virtual ~MessageBuffer(void);

	void clear(void);

	bool isFull(void) const;
	bool isEmpty(void) const;

	void setByteOrder(const ByteOrder& order);
	const ByteOrder& getByteOrder(void) const;

	uint32_t getCapacity(void) const;

	MessageBuffer& put(char ch);
	MessageBuffer& put(int16_t value);
	MessageBuffer& put(uint16_t value);
	MessageBuffer& put(int32_t value);
	MessageBuffer& put(uint32_t value);
	MessageBuffer& put(int64_t value);
	MessageBuffer& put(uint64_t value);
	MessageBuffer& put(const void* data, uint32_t len);

	char getByte(void);
	int16_t getShort(void);
	uint16_t getUShrot(void);
	int32_t getInt(void);
	uint32_t getUInt(void);
	int64_t getLong(void);
	uint64_t getULong(void);

	uint32_t readable(void) const;
	uint32_t writable(void) const;

	MessageBuffer& get(uint32_t len);
	MessageBuffer& add(uint32_t len);

	uint32_t fullForPut(mxos::IOVEC* vec, uint32_t count) const;
	uint32_t fullForGet(mxos::IOVEC* vec, uint32_t count) const;
private:
	MessageBuffer& putImpl(const void* data, uint32_t len);
	MessageBuffer& getImpl(void* data, uint32_t len);
private:
	char* buffer_;
	uint32_t capacity_;
	uint32_t readPos_;
	uint32_t writePos_;

	ByteOrder byteOrder_;
};

inline void MessageBuffer::clear(void)
{
	writePos_ = readPos_ = 0;
}

inline bool MessageBuffer::isFull(void) const
{
	return (writePos_ + 1) % capacity_ == readPos_;
}

inline bool MessageBuffer::isEmpty(void) const
{
	return readPos_ == writePos_;
}

inline void MessageBuffer::setByteOrder(const ByteOrder& order)
{
	byteOrder_ = order;
}

inline const ByteOrder& MessageBuffer::getByteOrder(void) const
{
	return byteOrder_;
}

inline uint32_t MessageBuffer::getCapacity(void) const
{
	return capacity_ - 1;
}

inline uint32_t MessageBuffer::readable(void) const
{
	if (readPos_ == writePos_)
	{
		return 0;
	}
	else if (readPos_ > writePos_)
	{
		return capacity_ - (readPos_ - writePos_);
	}
	else
	{
		return writePos_ - readPos_;
	}
}

inline uint32_t MessageBuffer::writable(void) const
{
	if (readPos_ == writePos_)
	{
		return capacity_ - 1;
	}
	else if (readPos_ > writePos_)
	{
		return readPos_ - writePos_ - 1;
	}
	else
	{
		return readPos_ + capacity_ - writePos_ - 1;
	}
}

typedef SharedPtr<MessageBuffer> MessageBufferPtr;

} // namespace mxcore

#endif /* __MXCORE_MESSAGEBUFFER_H__ */
