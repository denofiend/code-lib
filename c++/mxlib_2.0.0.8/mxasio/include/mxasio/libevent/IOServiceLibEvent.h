/*
 *        Name: IOServiceLibEvent.h
 *
 *  Created on: 2011-2-21
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXASIO_LIBEVENT_IOSERVICELIBEVENT_H__
#define __MXASIO_LIBEVENT_IOSERVICELIBEVENT_H__

#include <event.h>
#include <ext/hash_map>
#include <mxcore/LoggerFactory.h>

#include <mxasio/IOService.h>
#include <mxasio/libevent/Wakeuper.h>
#include <mxasio/libevent/ChannelContext.h>

namespace mxasio
{

class IOServiceLibEvent: public IOService
{
public:
	IOServiceLibEvent(void);

	virtual ~IOServiceLibEvent(void);

	void open(void) throw (mxcore::IOException&);

	virtual void close(bool dontCallOnClose = false);

	virtual bool isClosed(void) const;

	virtual void* getAttachment(const ChannelId& channelId) const;

	virtual void* setAttachment(const ChannelId& channelId, void* attachment);

	virtual ChannelId
	listen(IOApplication* application, const mxcore::SocketAddress& local,
			int backlog, bool reuseAddr, void* attachment)
			throw (mxcore::IOException&);

	virtual ChannelId listen(IOApplication* application, int sock,
			void* attachment) throw (mxcore::IOException&);

	virtual ChannelId connect(IOApplication* application,
			const mxcore::SocketAddress& remote, int timeout, void* attachment)
			throw (mxcore::IOException&);

	virtual ChannelId connect(IOApplication* application, int sock,
			void* attachment) throw (mxcore::IOException&);

	virtual void send(const ChannelId& id,
			mxcore::SharedPtr<mxcore::ByteBuffer> message, int timeout);

	virtual void
			send(
					const ChannelId& id,
					const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> > messageList,
					int perMsgTimeout);

	virtual void recv(const ChannelId& id,
			mxcore::SharedPtr<mxcore::ByteBuffer> message, int timeout,
			bool recvAll = false);

	virtual bool isChannelClosed(const ChannelId& id) const;

	virtual void
	closeChannel(const ChannelId& id, bool dontCallOnClose = false,
			bool closeSock = true);

	virtual void select(void) throw (mxcore::IOException&);

	virtual void wakeup(void) throw (mxcore::IOException&);

	struct event_base* getEventBase(void) const;
private:
	void doAccept(AcceptorChannelContext* ctx);
	void doConnect(ConnectorChannelContext* ctx);
	void doRead(ConnectorChannelContext* ctx);
	void doWrite(ConnectorChannelContext* ctx);

	void doConnectTimeout(ConnectorChannelContext* ctx);
	void doReadTimeout(ConnectorChannelContext* ctx);
	void doWriteTimeout(ConnectorChannelContext* ctx);

	void prepareForNextRead(ConnectorChannelContext* ctx);
	void prepareForNextWrite(ConnectorChannelContext* ctx);

	void
			handleRead(
					ConnectorChannelContext* ctx,
					const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& readMsgList);
	void handleReadError(ConnectorChannelContext* ctx,
			const mxcore::SharedPtr<mxcore::ByteBuffer>& msg, int errorCode);

	void
			handleWrite(
					ConnectorChannelContext* ctx,
					const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& writtenMsgList);

	void handleWriteError(ConnectorChannelContext* ctx,
			const mxcore::SharedPtr<mxcore::ByteBuffer>& msg, int errorCode);

	ConnectorChannelContext* registConnector(mxcore::Socket& connector,
			IOApplication* application, const std::string& local,
			const std::string& remote, void* attachment = NULL,
			bool connected = false);

	AcceptorChannelContext* registAcceptor(mxcore::ServerSocket& acceptor,
			IOApplication* application, const std::string& local,
			void* attachment = NULL);
public:
	static void onAccept(int fd, short event, void* arg);
	static void onRead(int fd, short event, void* arg);
	static void onWrite(int fd, short event, void* arg);
	static void onReadTimeout(int fd, short event, void* arg);
	static void onWriteTimeout(int fd, short event, void* arg);
private:
	ChannelContext* findChannel(const ChannelId& id) const;

	void closeImpl(ChannelContext* context, bool dontCallOnClose = false,
			bool closeSock = true);

	ChannelId genChannelId(int sock, const std::string& local,
			const std::string& remote);
private:
	struct event_base* evbase_;
	Wakeuper wakeuper_;

	typedef __gnu_cxx ::hash_map<ChannelId, ChannelContext*, ChannelIdHash,
			ChannelIdEqual> ChannelMap;

	ChannelMap channelMap_;

	uint64_t sequence_;
};

inline struct event_base* IOServiceLibEvent::getEventBase(void) const
{
	return evbase_;
}

} // namespace mxasio

#endif /* __MXASIO_LIBEVENT_IOSERVICELIBEVENT_H__ */
