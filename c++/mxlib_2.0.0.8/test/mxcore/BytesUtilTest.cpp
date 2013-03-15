/*
 *        Name: BytesUtilTest.cpp
 *
 *  Created on: 2011-2-15
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <iostream>
#include "BytesUtilTest.h"
#include <mxcore/Verify.h>
#include <mxcore/Assert.h>
#include <mxcore/ByteOrder.h>
#include <mxcore/SocketOps.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

BytesUtilTest::BytesUtilTest()
{
}

BytesUtilTest::~BytesUtilTest()
{
}

void BytesUtilTest::init(void)
{
}

void BytesUtilTest::perform(void)
{
	std::cout << "BytesUtilTest::perform ..." << std::endl;

	MX_VERIFY(ByteOrder::LITTLEENDIAN.isNativeOrder());
	MX_VERIFY(!ByteOrder::BIGENDIAN.isNativeOrder());

	int32_t value = 0x12345678;
	int64_t value64 = 0x1234567812345678LL;

	MX_VERIFY(ByteOrder::NATIVE_ORDER.toOrder(value, ByteOrder::LITTLEENDIAN) == value);
	MX_VERIFY(ByteOrder::NATIVE_ORDER.toOrder(value, ByteOrder::BIGENDIAN) == (int32_t)0x78563412);

	MX_VERIFY(ByteOrder::NATIVE_ORDER.toOrder(value64, ByteOrder::LITTLEENDIAN) == value64);
	MX_VERIFY(ByteOrder::NATIVE_ORDER.toOrder(value64, ByteOrder::BIGENDIAN) == (int64_t)0x7856341278563412LL);
	//	MX_VERIFY(ByteOrder::BIG_ENDIAN.fromNative(value) == value);
}

void BytesUtilTest::cleanup(void)
{
}

}
