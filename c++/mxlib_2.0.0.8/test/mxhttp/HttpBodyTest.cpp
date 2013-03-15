/*
 *        Name: HttpBodyTest.cpp
 *
 *  Created on: 2011-2-12
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <iostream>
#include "HttpBodyTest.h"
#include <mxhttp/HttpBody.h>
#include <mxhttp/HttpSimpleBody.h>
#include <mxcore/Verify.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpBodyTest::HttpBodyTest()
{
}

HttpBodyTest::~HttpBodyTest()
{
}

void HttpBodyTest::init(void)
{
}

void HttpBodyTest::perform(void)
{
	std::cout << "HttpBodyTest::perform ..." << std::endl;

	try
	{
		char buffer[10] =
		{ 0 };
		HttpSimpleBody* body = HttpSimpleBody::makeInFile("./body.txt", true);
		body->write("hello", 5);
		MX_VERIFY(body->getContentLength() == 5);
		body->flip();
		MX_VERIFY(body->getContentLength() == 5);
		MX_VERIFY(5 == body->read(buffer, 5));
		MX_VERIFY(-1 == body->read(buffer, 5));
		delete body;
	} catch (mxcore::IOException& e)
	{
		std::cout << e.name() << "," << e.getMessage() << ", "
				<< e.getFilename() << ":" << e.getLineNumber() << std::endl;
	}
}

void HttpBodyTest::cleanup(void)
{
}

}
