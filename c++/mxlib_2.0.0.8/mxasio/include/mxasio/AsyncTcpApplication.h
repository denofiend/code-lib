/*
 * AsyncTcpApplication.h
 *
 *  Created on: 2011-3-2
 *      Author: zhaojiangang
 */

#ifndef __MXASIO_ASYNCTCPAPPLICATION_H__
#define __MXASIO_ASYNCTCPAPPLICATION_H__

#include <mxasio/IOApplication.h>

namespace mxasio
{

class AsyncTcpServer;

class AsyncTcpApplication: public IOApplicationAdaptor
{
public:
	virtual ~AsyncTcpApplication(void)
	{
	}

	virtual void beforeSelect(AsyncTcpServer* tcpServer) = 0;

	virtual void afterSelect(AsyncTcpServer* tcpServer) = 0;

	virtual void shutdown(AsyncTcpServer* tcpServer) = 0;
};

} // namespace mxasio

#endif /* __MXASIO_ASYNCTCPAPPLICATION_H__ */
