/*
 * HttpResponser.h
 *
 *  Created on: 2011-3-1
 *      Author: zhaojiangang
 */

#ifndef __MXHTTP_HTTPRESPONSER_H__
#define __MXHTTP_HTTPRESPONSER_H__

#include <mxcore/SharedPtr.h>
#include <mxhttp/HttpMessage.h>

namespace mxhttp
{

class HttpResponser
{
public:
	virtual ~HttpResponser(void)
	{
	}

	virtual bool asyncResponse(mxcore::SharedPtr<HttpRequest> request,
			mxcore::SharedPtr<HttpResponse> response) = 0;

	virtual bool response(mxcore::SharedPtr<HttpRequest> request,
			mxcore::SharedPtr<HttpResponse> response) = 0;
};

} // namespace mxhttp

#endif /* __MXHTTP_HTTPRESPONSER_H__ */
