/*
 * HttpConnectionId.h
 *
 *  Created on: 2011-3-1
 *      Author: zhaojiangang
 */

#ifndef __MXHTTP_HTTPCONNECTIONID_H__
#define __MXHTTP_HTTPCONNECTIONID_H__

#include <mxasio/ChannelId.h>

namespace mxhttp
{

typedef mxasio::ChannelId HttpConnectionId;

struct HttpConnectionIdHash
{
	unsigned int operator()(const HttpConnectionId& id) const
	{
		return (unsigned int) id.getSequence();
	}
};

struct HttpConnectionIdEqual
{
	bool operator()(const HttpConnectionId& x, const HttpConnectionId& y) const
	{
		return x.getSequence() == y.getSequence() && x.getLocal()
				== y.getLocal() && x.getRemote() == y.getRemote();
	}
};

} // namespace mxhttp

#endif /* HTTPCONNECTIONID_H_ */
