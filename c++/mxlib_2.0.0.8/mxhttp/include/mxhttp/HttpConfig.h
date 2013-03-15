/*
 * HttpConfig.h
 *
 *  Created on: 2011-3-2
 *      Author: zhaojiangang
 */

#ifndef __MXHTTP_HTTPCONFIG_H__
#define __MXHTTP_HTTPCONFIG_H__

#include <list>
#include <string>
#include <stdint.h>
#include <mxcore/SocketAddress.h>
#include <mxcore/LoggerFactory.h>

namespace mxhttp
{

class HttpConfig
{
public:
	HttpConfig(void);
	HttpConfig(const HttpConfig& src);
	HttpConfig& operator =(const HttpConfig& other);
	~HttpConfig(void);
public:
	int64_t maxContentLength_;
	int64_t cachedLimitSize_;
	std::string tempRepository_;

	std::string username_;

	uint32_t subProcessNum_;
	uint32_t maxConnectionPerProcess_;

	int recvTimeout_;
	int sendTimeout_;

	int keepAliveTimeout_;

	uint32_t bufferSize_;

	bool forceKeepAvlie_;

	std::list<mxcore::SocketAddress> listenList_;
	std::list<mxcore::LoggerFactory::LoggerConfig> loggerList_;
};

} // namespace mxhttp

#endif /* __MXHTTP_HTTPCONFIG_H__ */
