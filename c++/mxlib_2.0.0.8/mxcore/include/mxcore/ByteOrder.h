/*
 *        Name: ByteOrder.h
 *
 *  Created on: 2011-2-15
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_BYTEORDER_H__
#define __MXCORE_BYTEORDER_H__

#include <stdint.h>

namespace mxcore
{

class ByteOrder
{
public:
	typedef enum
	{
		eBigEndian, eLittleEndian
	} Order;
	ByteOrder(Order order);
public:
	ByteOrder(const ByteOrder& src);
	ByteOrder& operator =(const ByteOrder& other);
	~ByteOrder(void);

	bool operator ==(const ByteOrder& other) const;
	bool isBigEndian(void) const;
	bool isLittleEndian(void) const;
	bool isNativeOrder(void) const;

	static const ByteOrder BIGENDIAN;
	static const ByteOrder LITTLEENDIAN;
	static const ByteOrder NATIVE_ORDER;

	int16_t toOrder(int16_t value, const ByteOrder& to) const;
	uint16_t toOrder(uint16_t value, const ByteOrder& to) const;
	int32_t toOrder(int32_t value, const ByteOrder& to) const;
	uint32_t toOrder(uint32_t value, const ByteOrder& to) const;
	int64_t toOrder(int64_t value, const ByteOrder& to) const;
	uint64_t toOrder(uint64_t value, const ByteOrder& to) const;
private:
	static uint16_t flipBytes(uint16_t value);
	static uint32_t flipBytes(uint32_t value);
	static uint64_t flipBytes(uint64_t value);
private:
	Order order_;
};

inline bool ByteOrder::operator ==(const ByteOrder& other) const
{
	return order_ == other.order_;
}

inline bool ByteOrder::isBigEndian(void) const
{
	return eBigEndian == order_;
}

inline bool ByteOrder::isLittleEndian(void) const
{
	return eLittleEndian == order_;
}

inline bool ByteOrder::isNativeOrder(void) const
{
	return order_ == NATIVE_ORDER.order_;
}

inline int16_t ByteOrder::toOrder(int16_t value, const ByteOrder& to) const
{
	if (order_ == to.order_)
	{
		return value;
	}

	return (int16_t) flipBytes((uint16_t) value);
}

inline uint16_t ByteOrder::toOrder(uint16_t value, const ByteOrder& to) const
{
	if (order_ == to.order_)
	{
		return value;
	}

	return flipBytes(value);
}

inline int32_t ByteOrder::toOrder(int32_t value, const ByteOrder& to) const
{
	if (order_ == to.order_)
	{
		return value;
	}

	return (int32_t) flipBytes((uint32_t) value);
}

inline uint32_t ByteOrder::toOrder(uint32_t value, const ByteOrder& to) const
{
	if (order_ == to.order_)
	{
		return value;
	}

	return flipBytes(value);
}

inline int64_t ByteOrder::toOrder(int64_t value, const ByteOrder& to) const
{
	if (order_ == to.order_)
	{
		return value;
	}

	return (int64_t) flipBytes((uint64_t) value);
}

inline uint64_t ByteOrder::toOrder(uint64_t value, const ByteOrder& to) const
{
	if (order_ == to.order_)
	{
		return value;
	}

	return flipBytes(value);
}

inline uint16_t ByteOrder::flipBytes(uint16_t value)
{
	return (((value >> 8) & 0x00FF) | ((value << 8) & 0xFF00));
}

inline uint32_t ByteOrder::flipBytes(uint32_t value)
{
	return (((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00)
			| ((value << 8) & 0x00FF0000) | ((value << 24) & 0xFF000000));
}

inline uint64_t ByteOrder::flipBytes(uint64_t value)
{
	uint32_t hi = uint32_t(value >> 32);
	uint32_t lo = uint32_t(value & 0xFFFFFFFF);

	uint64_t fhi = uint64_t(flipBytes(hi));
	uint64_t flo = uint64_t(flipBytes(lo));

	flo <<= 32;
	return (flo | fhi);
}

} // namespace mxcore

#endif /* BYTEORDER_H_ */
