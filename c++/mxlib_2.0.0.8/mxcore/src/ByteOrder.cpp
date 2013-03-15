/*
 *        Name: ByteOrder.cpp
 *
 *  Created on: 2011-2-15
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/ByteOrder.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

static ByteOrder::Order getNativeOrderType(void)
{
	static union
	{
		short int i;
		char c;
	} data =
	{ 1 };

	return 1 == data.c ? ByteOrder::eLittleEndian : ByteOrder::eBigEndian;
}

const ByteOrder ByteOrder::BIGENDIAN(ByteOrder::eBigEndian);
const ByteOrder ByteOrder::LITTLEENDIAN(ByteOrder::eLittleEndian);
const ByteOrder ByteOrder::NATIVE_ORDER(getNativeOrderType());

ByteOrder::ByteOrder(Order order) :
	order_(order)
{
}

ByteOrder::ByteOrder(const ByteOrder& src) :
	order_(src.order_)
{
}

ByteOrder& ByteOrder::operator =(const ByteOrder& other)
{
	if (this != &other)
	{
		order_ = other.order_;
	}
	return *this;
}

ByteOrder::~ByteOrder(void)
{
}

} // namespace mxcore
