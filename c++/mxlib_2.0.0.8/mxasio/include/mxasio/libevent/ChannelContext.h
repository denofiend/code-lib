/*
 *        Name: ChannelContextLibEvent.h
 *
 *  Created on: 2011-2-21
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXASIO_LIBEVENT_CHANNELCONTEXTLIBEVENT_H__
#define __MXASIO_LIBEVENT_CHANNELCONTEXTLIBEVENT_H__

#include <map>
#include <list>
#include <event.h>
#include <iostream>
#include <mxcore/Socket.h>
#include <mxcore/ServerSocket.h>

#include <mxasio/ChannelId.h>
#include <mxasio/IOApplication.h>
#include <mxasio/ChannelMessage.h>

namespace mxasio
{

class IOServiceLibEvent;

class ChannelContext
{
public:
	enum
	{
		eAcceptor, eConnector
	};

	ChannelContext(int type, IOServiceLibEvent* service,
			const ChannelId& channelId, IOApplication* application,
			void* attachment);
	virtual ~ChannelContext(void);

	IOServiceLibEvent* getService(void) const;
	const ChannelId& getChannelId(void) const;
	IOApplication* getApplication(void) const;
	void* getAttachment(void) const;

	void setAttachment(void* attachment);

	bool isAcceptor(void) const;
	bool isConnector(void) const;

	virtual void unregistAll(void) = 0;
private:
	int type_;
	ChannelId channelId_;
	IOServiceLibEvent* service_;

	IOApplication* application_;
	void* attachment_;
};

inline IOServiceLibEvent* ChannelContext::getService(void) const
{
	return service_;
}

inline const ChannelId& ChannelContext::getChannelId(void) const
{
	return channelId_;
}

inline IOApplication* ChannelContext::getApplication(void) const
{
	return application_;
}

inline void* ChannelContext::getAttachment(void) const
{
	return attachment_;
}

inline void ChannelContext::setAttachment(void* attachment)
{
	attachment_ = attachment;
}

class AcceptorChannelContext: public ChannelContext
{
public:
	AcceptorChannelContext(IOServiceLibEvent* service,
			const ChannelId& channelId, IOApplication* application,
			void* attachment);

	virtual ~AcceptorChannelContext(void);

	mxcore::ServerSocket& getAcceptor(void);

	void registAccept(void);
	void unregistAccept(void);

	virtual void unregistAll(void);
private:
	mxcore::ServerSocket acceptor_;

	struct event acceptEvt_;
};

inline void AcceptorChannelContext::unregistAll(void)
{
	unregistAccept();
}

inline mxcore::ServerSocket& AcceptorChannelContext::getAcceptor(void)
{
	return acceptor_;
}

class ConnectorChannelContext: public ChannelContext
{
public:
	ConnectorChannelContext(IOServiceLibEvent* service,
			const ChannelId& channelId, IOApplication* application,
			void* attachment);

	virtual ~ConnectorChannelContext(void);

	mxcore::Socket& getConnector(void);

	void registRead(void);
	void registReadTimer(int timeout);
	void registWrite(void);
	void registWriteTimer(int timeout);

	void unregistRead(void);
	void unregistReadTimer(void);
	void unregistWrite(void);
	void unregistWriteTimer(void);

	virtual void unregistAll(void);

	bool haveMessageToRead(void) const;
	bool haveMessageToWrite(void) const;

	int readMessage(
			std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& readMsgList)
			throw (mxcore::IOException&);

	void writeMessage(
			std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& writtenMsgList)
			throw (mxcore::IOException&);

	ChannelMessage frontReadMessage(void);
	ChannelMessage popFrontReadMessage(void);

	ChannelMessage frontWriteMessage(void);
	ChannelMessage popFrontWriteMessage(void);

	void popAllReadMessage(void);
	void popAllWriteMessage(void);

	void putReadMessage(const ChannelMessage& msg);
	void putWriteMessage(const ChannelMessage& msg);
	void
			putWriteMessage(
					const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> > messageList,
					int perMsgTimeout);
private:
	mxcore::Socket connector_;

	struct event readEvt_;
	struct event readTimer_;

	struct event writeEvt_;
	struct event writeTimer_;

	std::list<ChannelMessage> toReadMessageList_;
	std::list<ChannelMessage> toWriteMessageList_;
};

} // namespace mxasio

#endif /* __MXASIO_LIBEVENT_CHANNELCONTEXTLIBEVENT_H__ */
