/*
 * HttpVirtualServerTest.cpp
 *
 *  Created on: 2011-3-9
 *      Author: zhaojiangang
 */

#include <mxcore/Verify.h>
#include "HttpVirtualServerTest.h"
#include <mxhttp/HttpVirtualServer.h>
#include <mxhttp/HttpFileDownLoadServlet.h>
#include <mxhttp/HttpSingleFileUploadServlet.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpVirtualServerTest::HttpVirtualServerTest()
{
}

HttpVirtualServerTest::~HttpVirtualServerTest()
{
	// TODO Auto-generated destructor stub
}

void HttpVirtualServerTest::init(void)
{
}

void HttpVirtualServerTest::perform(void)
{
	mxcore::SharedPtr<HttpVirtualServer> vs = new HttpVirtualServer(
			"www.google.com", "/Users/zhaojiangang");

	mxcore::SharedPtr<HttpServlet> servlet = new HttpSingleFileUploadServlet;

	std::map<std::string, std::vector<std::string> > params;
	mxcore::SharedPtr<HttpServlet> downloadServlet = new HttpFileDownLoadServlet;

	params["allow"].push_back("^/test.txt");
	params["denine"].push_back("^/denine.txt");

	downloadServlet->setParams(params);
	downloadServlet->init();
	vs->setFiledownloadSetvlet(downloadServlet);

	MX_VERIFY(vs->addServlet("^/sayhello/\\d{1,10}/\\d{1,10}", servlet));
	MX_VERIFY(!vs->addServlet("^/sayhello/\\d{1,10}/\\d{1,10}", servlet));

	HttpRequest request;
	request.setMethod("GET");
	request.setUri("/sayhello/123/456");
	MX_VERIFY(vs->findServlet(request) == servlet);

	request.setUri("/test.html");
	MX_VERIFY(vs->findServlet(request) == downloadServlet);
}
void HttpVirtualServerTest::cleanup(void)
{
}
}
