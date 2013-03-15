/*
 *        Name: IOService.h
 *
 *  Created on: 2011-2-18
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXASIO_IOSERVICE_H__
#define __MXASIO_IOSERVICE_H__

#include <list>
#include <mxasio/ChannelId.h>

#include <mxcore/SharedPtr.h>
#include <mxcore/ByteBuffer.h>
#include <mxcore/SocketAddress.h>

namespace mxasio
{

class IOApplication;

class IOService
{
public:
	virtual ~IOService(void)
	{
	}

	static IOService* createInstance(void) throw (mxcore::IOException&);

	virtual void close(bool dontCallOnClose = false) = 0;

	virtual bool isClosed(void) const = 0;

	virtual void* getAttachment(const ChannelId& channelId) const = 0;

	virtual void* setAttachment(const ChannelId& channelId, void* attachment) = 0;

	virtual ChannelId
	listen(IOApplication* application, const mxcore::SocketAddress& local,
			int backlog, bool reuseAddr, void* attachment)
			throw (mxcore::IOException&) = 0;

	virtual ChannelId listen(IOApplication* application, int sock,
			void* attachment) throw (mxcore::IOException&) = 0;

	virtual ChannelId connect(IOApplication* application,
			const mxcore::SocketAddress& remote, int timeout, void* attachment)
			throw (mxcore::IOException&) = 0;

	virtual ChannelId connect(IOApplication* application, int sock,
			void* attachment) throw (mxcore::IOException&) = 0;

	virtual void send(const ChannelId& id,
			mxcore::SharedPtr<mxcore::ByteBuffer> message, int timeout) = 0;

	virtual void
			send(
					const ChannelId& id,
					const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> > messageList,
					int perMsgTimeout) = 0;

	virtual void recv(const ChannelId& id,
			mxcore::SharedPtr<mxcore::ByteBuffer> message, int timeout,
			bool recvAll = false) = 0;

	virtual bool isChannelClosed(const ChannelId& id) const = 0;

	virtual void
	closeChannel(const ChannelId& id, bool dontCallOnClose = false,
			bool closeSock = true) = 0;

	virtual void select(void) throw (mxcore::IOException&) = 0;

	virtual void wakeup(void) throw (mxcore::IOException&) = 0;
};

} // namespace mxasio

#endif /* __MXASIO_IOSERVICE_H__ */
