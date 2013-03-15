/*
 * HttpServlet.h
 *
 *  Created on: 2011-3-1
 *      Author: zhaojiangang
 */

#ifndef __MXHTTP_HTTPSERVLET_H__
#define __MXHTTP_HTTPSERVLET_H__

#include <map>
#include <vector>
#include <string>
#include <mxasio/IOService.h>
#include <mxcore/MutexLock.h>
#include <mxcore/Exception.h>
#include <mxcore/SharedPtr.h>
#include <mxhttp/HttpResponser.h>

namespace mxhttp
{

class HttpVirtualServer;

class HttpServlet
{
public:
	virtual ~HttpServlet(void)
	{
	}

	virtual void setParams(
			const std::map<std::string, std::vector<std::string> >& params) = 0;

	virtual void init(void) throw (mxcore::Exception&) = 0;

	virtual void
			service(mxcore::SharedPtr<HttpVirtualServer> vs,
					mxcore::SharedPtr<HttpRequest> request,
					HttpResponser* responser) = 0;
};

class HttpAbstractServlet: public HttpServlet
{
public:
	HttpAbstractServlet(void);
	~HttpAbstractServlet(void);

	virtual void init(void) throw (mxcore::Exception&);

	virtual void setParams(
			const std::map<std::string, std::vector<std::string> >& params);
protected:
	virtual void initImpl(void) throw (mxcore::Exception&);
protected:
	bool initFlag_;
	mxcore::AutoMutexLock initLock_;
	std::map<std::string, std::vector<std::string> > params_;
};

} // namespace mxhttp

#endif /* __MXHTTP_HTTPSERVLET_H__ */
