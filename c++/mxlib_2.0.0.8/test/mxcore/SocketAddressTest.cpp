/*
 *        Name: SocketAddressTest.cpp
 *
 *  Created on: 2011-2-11
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <iostream>
#include "SocketAddressTest.h"
#include <mxcore/SocketAddress.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

SocketAddressTest::SocketAddressTest()
{
}

SocketAddressTest::~SocketAddressTest()
{
}

void SocketAddressTest::init(void)
{
}

void SocketAddressTest::perform(void)
{
	std::cout << "SocketAddressTest::perform ..." << std::endl;

	try
	{
		SocketAddress socket_address;
		socket_address.set("www.sina.com.cn", AF_INET, 80);

		std::cout << "addr:" << socket_address.toString() << std::endl;
	} catch (Exception& e)
	{
		std::cout << e.getMessage() << std::endl;
	}
}

void SocketAddressTest::cleanup(void)
{
}

}
