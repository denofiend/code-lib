/*
 *
 *        Name: HttpParserTest.cpp
 *  Created on: 2011-2-24
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <fstream>
#include <iostream>
#include <mxcore/Verify.h>
#include <mxcore/ByteBuffer.h>
#include "HttpParserTest.h"
#include "mxhttp/HttpMsgParser.h"
#include "mxhttp/TempFileNameGenerator.h"
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpParserTest::HttpParserTest()
{
}

HttpParserTest::~HttpParserTest()
{
}

void HttpParserTest::init(void)
{
}

static void parse_valid_request1(HttpMsgParser& parser)
{
	{
		mxcore::ByteBuffer buffer(1024);
		std::ifstream f;
		f.open("mxhttp/request/valid_request1.txt", std::fstream::binary);

		while (!f.fail() && !f.eof())
		{
			int rem = buffer.remaining();

			if (0 == rem)
			{
				break;
			}

			f.read((char*) buffer.getBuffer(), buffer.remaining());
			int r = f.gcount();

			buffer.setPosition(buffer.getPosition() + r);

			buffer.flip();

			int parsedLen = parser.append(buffer.getBuffer(), r);

			buffer.setPosition(buffer.getPosition() + parsedLen);

			rem = buffer.remaining();

			if (rem > 0)
			{
				::memmove(buffer.getOrgBuffer(), buffer.getBuffer(), rem);
				buffer.setPosition(rem);
				buffer.setLimit(buffer.getCapacity());
			}
			else
			{
				buffer.clear();
			}

			if (parser.isComplated())
			{
				HttpRequest* request = parser.getRequest();

				MX_VERIFY(request->getMethod() == std::string("GET"));
				MX_VERIFY(request->getVersion() == std::string("HTTP/1.1"));
				MX_VERIFY(request->getUri() == std::string("/index.php"));
				MX_VERIFY(request->getParams().getValue("tn") == std::string("maxthon2"));
				MX_VERIFY(request->getParams().getValue("ch") == std::string("3"));

				const char
						* cookie =
								"BAIDUID=433A54C5CF294A81BB44D74914414CD8:FG=1;"
									" USERID=6340af700d0b50f3d86b; _ML_HIDE=1_;BDUSS=WlTcFlUd3ZCNTViMGhPNn50aHZIM0s2SXFQNjd"
									"6WlBjWkJiY0l6em9UWnZ1MzFOQVFBQUFBJCQAAAAAAAAAAAokNw-STI4MempnY2NsAAAAAAAAAAAAAAAAAAAAAA"
									"AAAAAAAAAAAADAekV4AAAAAMB6RXgAAAAAuFNCAAAAAAAxMC42NS4yNG8uVk1vLlZNc";

				MX_VERIFY(request->getHeaders().getValue("Accept") == std::string("*/*"));
				MX_VERIFY(request->getHeaders().getValue("Accept-Language") == std::string("zh-cn"));
				MX_VERIFY(request->getHeaders().getValue("Accept-Encoding") == std::string("gzip, deflate"));
				MX_VERIFY(request->getHeaders().getValue("User-Agent") == std::string("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727; CIBA; .NET CLR 1.1.4322)"));
				MX_VERIFY(request->getHeaders().getValue("Host") == std::string("www.baidu.com"));
				MX_VERIFY(request->isKeepAlive());
				MX_VERIFY(request->getHeaders().getValue("Connection") == std::string("Keep-Alive"));
				MX_VERIFY(request->getHeaders().getValue("Cookie") == cookie);
				MX_VERIFY(request->getBody() == NULL);
				break;
			}

		}

		MX_VERIFY(parser.isComplated());
	}
}

static void parse_valid_multipart_request(HttpMsgParser& parser)
{
	parser.clear();
	mxcore::ByteBuffer buffer(1024);
	std::ifstream f;
	f.open("mxhttp/request/valid_multipart_request.txt",
			std::ios::binary | std::ios::in);

	while (!f.fail() && !f.eof())
	{
		int rem = buffer.remaining();

		if (0 == rem)
		{
			break;
		}

		f.read((char*) buffer.getBuffer(), buffer.remaining());
		int r = f.gcount();

		buffer.setPosition(buffer.getPosition() + r);

		buffer.flip();

		int parsedLen = parser.append(buffer.getBuffer(), r);

		buffer.setPosition(buffer.getPosition() + parsedLen);

		rem = buffer.remaining();

		if (rem > 0)
		{
			::memmove(buffer.getOrgBuffer(), buffer.getBuffer(), rem);
			buffer.setPosition(rem);
			buffer.setLimit(buffer.getCapacity());
		}
		else
		{
			buffer.clear();
		}

		if (parser.isComplated())
		{
			HttpRequest* request = parser.getRequest();

			MX_VERIFY(request->getMethod() == "POST");
			MX_VERIFY(request->getVersion() == "HTTP/1.1");
			MX_VERIFY(request->getUri() == "/upload.html");
			MX_VERIFY(request->getParams().getCount() == 0);

			MX_VERIFY(request->getHeaders().getValue("Accept") == "application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5");
			MX_VERIFY(request->getHeaders().getValue("Accept-Language") == "zh-CN");
			MX_VERIFY(request->getHeaders().getValue("Accept-Encoding") == "gzip,deflate");
			MX_VERIFY(request->getHeaders().getValue("User-Agent") == "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US) AppleWebKit/533.9 (KHTML, like Gecko) Maxthon/3.0 Safari/533.9");

			MX_VERIFY(request->getHeaders().getValue("Host") == "192.168.1.9:8080");
			MX_VERIFY(request->getBody() != NULL);

			MX_VERIFY(request->getBody()->isMultipartBody());

			HttpMultipartBody* body = (HttpMultipartBody*) request->getBody();

			MX_VERIFY(body->getItemCount() == 2);

			HttpFileItem* item = body->getItem(0);
			MX_VERIFY(item->isFormField());
			MX_VERIFY(item->getName() == "username");
			MX_VERIFY(((HttpFormFieldItem*)item)->getValue() == "123");

			item = body->getItem(1);
			MX_VERIFY(item->isFormField());
			MX_VERIFY(item->getName() == "password");
			MX_VERIFY(((HttpFormFieldItem*)item)->getValue() == "456");
			break;
		}
	}

	MX_VERIFY(parser.isComplated());
}

void HttpParserTest::perform(void)
{
	std::cout << "HttpParserTest::perform ..." << std::endl;

	try
	{
		TempFileNameGeneratorSimple tempFileNameGen("./");
		HttpMsgParser parser(409600, -1, &tempFileNameGen);

		parse_valid_request1(parser);
		parse_valid_multipart_request(parser);
		parser.clear();
	} catch (mxcore::Exception& e)
	{
		std::cout << e.name() << " " << e.getFilename() << ":"
				<< e.getLineNumber() << ", " << e.getMessage() << std::endl;
	}
}

void HttpParserTest::cleanup(void)
{
}

}
