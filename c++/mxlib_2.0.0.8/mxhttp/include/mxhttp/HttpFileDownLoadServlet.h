/*
 * HttpFileDownLoadServlet.h
 *
 *  Created on: 2011-3-8
 *      Author: zhaojiangang
 */

#ifndef __MXHTTP_HTTPFILEDOWNLOADSERVLET_H__
#define __MXHTTP_HTTPFILEDOWNLOADSERVLET_H__

#include <list>
#include <mxhttp/PcreUtil.h>
#include <mxhttp/HttpServlet.h>

namespace mxhttp
{

class HttpFileDownLoadServlet: public HttpAbstractServlet
{
public:
	HttpFileDownLoadServlet(void);

	virtual ~HttpFileDownLoadServlet(void);

	virtual void
	service(mxcore::SharedPtr<HttpVirtualServer> vs,
			mxcore::SharedPtr<HttpRequest> request, HttpResponser* responser);

protected:
	virtual void initImpl(void) throw (mxcore::Exception&);
private:
	bool doFilter(const std::string& uri, std::list<pcre*>& list);

	bool canAccess(const std::string& uri);

	void addFilters(std::list<pcre*>& li,
			const std::vector<std::string>& params) throw (mxcore::Exception&);
private:
	std::list<pcre*> allowList_;
	std::list<pcre*> denineList_;
};

} // namespace mxhttp

#endif /* __MXHTTP_HTTPFILEDOWNLOADSERVLET_H__ */
