/*
 * HttpConfig.cpp
 *
 *  Created on: 2011-3-2
 *      Author: zhaojiangang
 */

#include <mxhttp/HttpConfig.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpConfig::HttpConfig(void)
{
}

HttpConfig::HttpConfig(const HttpConfig& src) :
	maxContentLength_(src.maxContentLength_),
			cachedLimitSize_(src.cachedLimitSize_),
			tempRepository_(src.tempRepository_), username_(src.username_),
			subProcessNum_(src.subProcessNum_),
			maxConnectionPerProcess_(src.maxConnectionPerProcess_),
			recvTimeout_(src.recvTimeout_), sendTimeout_(src.sendTimeout_),
			keepAliveTimeout_(src.keepAliveTimeout_),
			bufferSize_(src.bufferSize_), forceKeepAvlie_(src.forceKeepAvlie_),
			listenList_(src.listenList_), loggerList_(src.loggerList_)
{
}

HttpConfig& HttpConfig::operator =(const HttpConfig& other)
{
	if (this != &other)
	{
		maxContentLength_ = other.maxContentLength_;
		cachedLimitSize_ = other.cachedLimitSize_;
		tempRepository_ = other.tempRepository_;
		username_ = other.username_;
		subProcessNum_ = other.subProcessNum_;
		maxConnectionPerProcess_ = other.maxConnectionPerProcess_;
		recvTimeout_ = other.recvTimeout_;
		sendTimeout_ = other.sendTimeout_;
		keepAliveTimeout_ = other.keepAliveTimeout_;
		bufferSize_ = other.bufferSize_;
		forceKeepAvlie_ = other.forceKeepAvlie_;
		listenList_ = other.listenList_;
		loggerList_ = other.loggerList_;
	}
	return *this;
}

HttpConfig::~HttpConfig(void)
{
}

} // namespace mxhttp
