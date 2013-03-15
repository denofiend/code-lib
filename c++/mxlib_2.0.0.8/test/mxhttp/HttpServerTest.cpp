/*
 * HttpServerTest.cpp
 *
 *  Created on: 2011-3-3
 *      Author: zhaojiangang
 */

#include <iostream>
#include "HttpServerTest.h"
#include <mxcore/SystemOps.h>
#include <mxcore/ScopePtr.h>
#include <mxcore/GenernicFactory.h>

#include <mxhttp/HttpUtil.h>
#include <mxhttp/HttpConfigurationXml.h>

#include <mxcore/MemRecord.h>

using namespace std;
namespace mxhttp
{

HttpServerTest::HttpServerTest()
{
	// TODO Auto-generated constructor stub

}

HttpServerTest::~HttpServerTest()
{
	// TODO Auto-generated destructor stub
}

void HttpServerTest::init(void)
{
}

class ShutdownServlet: public HttpAbstractServlet
{
public:
	ShutdownServlet(void)
	{
	}

	virtual void service(mxcore::SharedPtr<HttpVirtualServer> vs,
			mxcore::SharedPtr<HttpRequest> request, HttpResponser* responser)
	{
		mxcore::SharedPtr<HttpResponse> response = HttpUtil::makeResponse(
				*request, HttpConstants::STATUS_OK, NULL);
		response->getHeaders().add(
				std::make_pair(HttpConstants::HEADER_CONTENT_TYPE, "text/html"));
		HttpSimpleBody* body = HttpSimpleBody::makeInMemory(
				"<html><body>ShutDown</body></html>");
		body->flip();
		response->setBody(body);
		responser->asyncResponse(request, response);

		std::cout << "before shutdown" << std::endl;
		vs->getServer()->shutdown();
	}
};

class SayHelloServlet: public HttpAbstractServlet
{
public:
	virtual void service(mxcore::SharedPtr<HttpVirtualServer> vs,
			mxcore::SharedPtr<HttpRequest> request, HttpResponser* responser)
	{
		mxcore::SharedPtr<HttpResponse> response = HttpUtil::makeResponse(
				*request, HttpConstants::STATUS_OK, NULL);
		response->getHeaders().add(
				std::make_pair(HttpConstants::HEADER_CONTENT_TYPE, "text/html"));
		std::string useridAndTimestamp = request->getUri().substr(10);
		std::string userid = useridAndTimestamp.substr(0,
				useridAndTimestamp.find('/'));
		std::string timestamp = useridAndTimestamp.substr(userid.length() + 1);
		std::string str("<html><body>hello: ");
		str += userid;
		str += "<br>timestamp: ";
		str += timestamp;
		str += "";
		str += "</body></html>";
		HttpSimpleBody* body = HttpSimpleBody::makeInMemory(str);
		body->flip();
		response->setBody(body);
		responser->asyncResponse(request, response);
	}
};

static mxcore::RegisterInFactory<HttpServlet, SayHelloServlet, std::string>
		regSayHelloServlet("mxhttp::SayHelloServlet");

static mxcore::RegisterInFactory<HttpServlet, ShutdownServlet, std::string>
		regShutdownServlet("mxhttp::ShutdownServlet");

void HttpServerTest::perform(void)
{
	std::cout << "HttpServerTest::perform ..." << std::endl;

	{
		//		HttpConfig cfg;
		//		cfg.maxContentLength_ = 409600;
		//		cfg.cachedLimitSize_ = 40960;
		//		cfg.tempRepository_ = "./";
		//		cfg.username_ = "zhaojiangang";
		//		cfg.subProcessNum_ = 4;
		//		cfg.recvTimeout_ = 5000;
		//		cfg.sendTimeout_ = 100000;
		//		cfg.keepAliveTimeout_ = 100000;
		//		cfg.bufferSize_ = 4096;
		//		cfg.maxConnectionPerProcess_ = 2;
		//
		//		mxcore::SocketAddress listenAddress;
		//		listenAddress.set("192.168.1.99", AF_INET, 10000);
		//
		//		cfg.forceKeepAvlie_ = false;
		//		cfg.listenList_.push_back(listenAddress);
		//
		//		listenAddress.set("192.168.1.99", AF_INET, 20000);
		//		cfg.listenList_.push_back(listenAddress);

		HttpConfigurationXml configuration("mxhttp/conf/mx_httpserv.xml");
		//		HttpServer* httpServer = HttpServer::createInstance(cfg);

		try
		{
			mxcore::ScopePtr<HttpServer> httpServer(
					configuration.buildInstance());
			httpServer->open();
			httpServer->run();
		} catch (mxcore::Exception& e)
		{
			std::cout << e.name() << ", " << e.getMessage() << ", "
					<< e.getErrorCode() << ", " << e.getFilename() << ":"
					<< e.getLineNumber() << std::endl;
			MX_ASSERT(0);
		}
	}
}

void HttpServerTest::cleanup(void)
{
}

}
