/*
 * HttpServlet.cpp
 *
 *  Created on: 2011-3-8
 *      Author: zhaojiangang
 */
#include <mxhttp/HttpServlet.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpAbstractServlet::HttpAbstractServlet(void) :
	initFlag_(false)
{
}

HttpAbstractServlet::~HttpAbstractServlet(void)
{
}

void HttpAbstractServlet::init(void) throw (mxcore::Exception&)
{
	if (!initFlag_)
	{
		mxcore::Lock::ScopeLock sl(initLock_);

		if (!initFlag_)
		{
			initImpl();
			initFlag_ = true;
		}
	}
}

void HttpAbstractServlet::setParams(
		const std::map<std::string, std::vector<std::string> >& params)
{
	params_ = params;
}

void HttpAbstractServlet::initImpl(void) throw (mxcore::Exception&)
{
}

} // namespace mxhttp
