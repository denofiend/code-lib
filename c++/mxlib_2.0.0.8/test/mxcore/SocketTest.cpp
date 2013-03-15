/*
 *        Name: SocketTest.cpp
 *
 *  Created on: 2011-2-12
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include "SocketTest.h"
#include <string>
#include <iostream>
#include <mxcore/Socket.h>
#include <mxcore/ServerSocket.h>
#include <mxcore/Verify.h>
#include <mxcore/IOUtil.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

SocketTest::SocketTest()
{
}

SocketTest::~SocketTest()
{
}

void SocketTest::init(void)
{
}

void SocketTest::perform(void)
{
	std::cout << "SocketTest::perform ..." << std::endl;

	try
	{
		int pair[2] =
		{ -1 };
		Socket client;
		ServerSocket server;
		SocketAddress local = SocketAddress::loopback(AF_INET, 20000);

		server.listen(local, 1, false);
		client.connect(server.getLocalAddress(), true);

		IOUtil::createSocketPair(pair);

		MX_VERIFY(-1 != pair[0] && -1 != pair[1]);

		char buf[6] =
		{ 0 };
		std::string str("hello");
		mxos::socketWrite(pair[0], str.c_str(), str.length());
		mxos::socketReadN(pair[1], buf, 5);
		mxos::closeSocket(pair[0]);
		mxos::closeSocket(pair[1]);

		MX_ASSERT(str == buf);
	} catch (IOException& e)
	{
		std::cout << e.getFilename() << ":" << e.getLineNumber() << ","
				<< e.getMessage() << "," << e.getErrorCode() << std::endl;
	}
}

void SocketTest::cleanup(void)
{
}

}
