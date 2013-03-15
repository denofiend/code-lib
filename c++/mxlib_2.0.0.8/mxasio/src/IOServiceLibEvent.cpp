/*
 *        Name: IOServiceLibEvent.cpp
 *
 *  Created on: 2011-2-21
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/IOUtil.h>
#include <mxcore/SocketOps.h>
#include <mxasio/libevent/ChannelContext.h>
#include <mxasio/libevent/IOServiceLibEvent.h>
#include <mxcore/MemRecord.h>

namespace mxasio
{

static mxcore::Logger& getLogger(void)
{
	return mxcore::LoggerFactory::getInstance().getLogger("mxasio.ioservice");
}

IOServiceLibEvent::IOServiceLibEvent(void) :
	evbase_(NULL), sequence_(0)
{
}

IOServiceLibEvent::~IOServiceLibEvent(void)
{
	close();
}

void IOServiceLibEvent::open(void) throw (mxcore::IOException&)
{
	close();

	evbase_ = ::event_base_new();

	if (NULL == evbase_)
	{
		THROW2(mxcore::IOException, "Can't open the event base");
	}

	try
	{
		wakeuper_.open(evbase_);
	} catch (mxcore::IOException& e)
	{
		::event_base_free(evbase_);
		evbase_ = NULL;
		throw e;
	}
}

void IOServiceLibEvent::close(bool dontCallOnClose)
{
	if (NULL != evbase_)
	{
		wakeuper_.close();

		while (!channelMap_.empty())
		{
			ChannelMap::iterator it = channelMap_.begin();
			ChannelContext* ctx = it->second;
			channelMap_.erase(it);
			closeImpl(ctx, dontCallOnClose);
		}

		::event_base_free(evbase_);
		evbase_ = NULL;
	}
}

bool IOServiceLibEvent::isClosed(void) const
{
	return NULL == evbase_;
}

void* IOServiceLibEvent::getAttachment(const ChannelId& channelId) const
{
	ChannelContext* ctx = findChannel(channelId);

	return (NULL == ctx) ? NULL : ctx->getAttachment();
}

void* IOServiceLibEvent::setAttachment(const ChannelId& channelId,
		void* attachment)
{
	ChannelContext* ctx = findChannel(channelId);

	MX_ASSERT(NULL != ctx);

	void* oldAttachment = ctx->getAttachment();

	ctx->setAttachment(attachment);

	return oldAttachment;
}

ChannelId IOServiceLibEvent::listen(IOApplication* application,
		const mxcore::SocketAddress& local, int backlog, bool reuseAddr,
		void* attachment) throw (mxcore::IOException&)
{
	mxcore::ServerSocket acceptor;
	acceptor.listen(local, backlog, reuseAddr, false);

	AcceptorChannelContext* ctx = registAcceptor(acceptor, application,
			local.toString(), attachment);

	ctx->registAccept();

	return ctx->getChannelId();
}

ChannelId IOServiceLibEvent::listen(IOApplication* application, int sock,
		void* attachment) throw (mxcore::IOException&)
{
	mxcore::ServerSocket acceptor;
	acceptor.attach(sock, true);
	acceptor.configBlocking(false);

	std::string local = acceptor.getLocalAddress().toString();

	AcceptorChannelContext* ctx = registAcceptor(acceptor, application, local,
			attachment);

	ctx->registAccept();

	return ctx->getChannelId();
}

ChannelId IOServiceLibEvent::connect(IOApplication* application,
		const mxcore::SocketAddress& remote, int timeout, void* attachment)
		throw (mxcore::IOException&)
{
	mxcore::Socket connector;
	connector.connect(remote, false);
	std::string local = connector.getLocalAddress().toString();

	ConnectorChannelContext* ctx = registConnector(connector, application,
			local, remote.toString(), attachment);

	ctx->registWrite();

	if (!connector.isConnected())
	{
		ctx->registWriteTimer(timeout);
	}
	return ctx->getChannelId();
}

ChannelId IOServiceLibEvent::connect(IOApplication* application, int sock,
		void* attachment) throw (mxcore::IOException&)
{
	mxcore::Socket connector;
	connector.attach(sock, false, true);

	std::string local = connector.getLocalAddress().toString();
	std::string remote = connector.getRemoteAddress().toString();

	connector.configBlocking(false);

	ConnectorChannelContext* ctx = registConnector(connector, application,
			local, remote, attachment);

	ctx->registWrite();

	return ctx->getChannelId();
}

void IOServiceLibEvent::send(const ChannelId& id,
		mxcore::SharedPtr<mxcore::ByteBuffer> message, int timeout)
{
	ChannelContext* ctx = findChannel(id);

	MX_ASSERT(NULL != ctx);
	MX_ASSERT(ctx->isConnector());
	MX_ASSERT(message->remaining() > 0);

	ConnectorChannelContext* connectorCtx = (ConnectorChannelContext*) ctx;

	bool haveMessage = connectorCtx->haveMessageToWrite();

	connectorCtx->putWriteMessage(ChannelMessage(message, timeout, false));

	if (!haveMessage)
	{
		connectorCtx->registWrite();
		connectorCtx->registWriteTimer(timeout);
	}
}

void IOServiceLibEvent::send(const ChannelId& id,
		const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> > messageList,
		int perMsgTimeout)
{
	ChannelContext* ctx = findChannel(id);

	MX_ASSERT(NULL != ctx);
	MX_ASSERT(ctx->isConnector());

	for (std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >::const_iterator it =
			messageList.begin(); it != messageList.end(); it++)
	{
		MX_ASSERT((*it)->remaining() > 0);
	}

	ConnectorChannelContext* connectorCtx = (ConnectorChannelContext*) ctx;

	bool haveMessage = connectorCtx->haveMessageToWrite();

	connectorCtx->putWriteMessage(messageList, perMsgTimeout);

	if (!haveMessage)
	{
		connectorCtx->registWrite();
		connectorCtx->registWriteTimer(perMsgTimeout);
	}
}

void IOServiceLibEvent::recv(const ChannelId& id,
		mxcore::SharedPtr<mxcore::ByteBuffer> message, int timeout,
		bool recvAll)
{
	ChannelContext* ctx = findChannel(id);

	MX_ASSERT(NULL != ctx);
	MX_ASSERT(ctx->isConnector());
	MX_ASSERT(message->remaining() > 0);

	ConnectorChannelContext* connectorCtx = (ConnectorChannelContext*) ctx;

	bool haveMessage = connectorCtx->haveMessageToRead();

	connectorCtx->putReadMessage(ChannelMessage(message, timeout, recvAll));

	if (!haveMessage)
	{
		connectorCtx->registRead();
		connectorCtx->registReadTimer(timeout);
	}
}

bool IOServiceLibEvent::isChannelClosed(const ChannelId& id) const
{
	return NULL == findChannel(id);
}

void IOServiceLibEvent::closeChannel(const ChannelId& id, bool dontCallOnClose,
		bool closeSock)
{
	ChannelMap::iterator it = channelMap_.find(id);

	if (it != channelMap_.end())
	{
		ChannelContext* ctx = it->second;
		channelMap_.erase(it);
		closeImpl(ctx, dontCallOnClose, closeSock);
	}
}

void IOServiceLibEvent::select(void) throw (mxcore::IOException&)
{
	MX_ASSERT(!isClosed());

	::event_base_loop(evbase_, EVLOOP_ONCE);
}

void IOServiceLibEvent::wakeup(void) throw (mxcore::IOException&)
{
	MX_ASSERT(!isClosed());
	wakeuper_.wakeup();
}

void IOServiceLibEvent::doAccept(AcceptorChannelContext* ctx)
{
	try
	{
		bool callClosed = false;
		mxcore::Socket client;

		if (!ctx->getAcceptor().accept(client))
		{
			return;
		}

		std::string local = client.getLocalAddress().toString();
		std::string remote = client.getRemoteAddress().toString();

		client.configBlocking(false);

		ConnectorChannelContext* connectorCtx = registConnector(client,
				ctx->getApplication(), local, remote, NULL, true);

		ChannelId newChannelId = connectorCtx->getChannelId();

		getLogger().debug("Local [%s] Accept client [%s]\n",
				ctx->getChannelId().toString().c_str(),
				newChannelId.toString().c_str());

		void* attachment = ctx->getApplication()->onNewClient(this,
				ctx->getChannelId(), ctx->getAttachment(), newChannelId,
				callClosed);

		if (!callClosed)
		{
			connectorCtx->setAttachment(attachment);
		}
	} catch (mxcore::IOException& e)
	{
		getLogger().error2(e.getErrorCode(),
				"IOServiceLibEvent::doAccept: %s %s:%d\n",
				e.getMessage().c_str(), e.getFilename().c_str(),
				e.getErrorCode());
	}
}

void IOServiceLibEvent::doConnect(ConnectorChannelContext* ctx)
{
	try
	{
		ctx->unregistWrite();
		ctx->unregistWriteTimer();
		ctx->getConnector().finishConnect();
		ctx->getApplication()->onConnect(this, ctx->getChannelId(),
				ctx->getAttachment());
	} catch (mxcore::IOException& e)
	{
		ctx->getApplication()->onConnectError(this, ctx->getChannelId(),
				ctx->getAttachment(), e.getErrorCode());
	}
}

void IOServiceLibEvent::doRead(ConnectorChannelContext* ctx)
{
	std::list<mxcore::SharedPtr<mxcore::ByteBuffer> > readMsgList;

	try
	{
		int r = ctx->readMessage(readMsgList);
		if (r > 0)
		{
			handleRead(ctx, readMsgList);
		}
		else
		{
			handleReadError(ctx, readMsgList.front(),
					mxasio::IOApplication::ERR_EOF);
		}
	} catch (mxcore::IOException& e)
	{
		handleReadError(ctx, readMsgList.front(), e.getErrorCode());
	}
}

void IOServiceLibEvent::doWrite(ConnectorChannelContext* ctx)
{
	std::list<mxcore::SharedPtr<mxcore::ByteBuffer> > writtenMsgList;

	try
	{
		ctx->writeMessage(writtenMsgList);
		handleWrite(ctx, writtenMsgList);
	} catch (mxcore::IOException& e)
	{
		handleWriteError(ctx, writtenMsgList.front(), e.getErrorCode());
	}
}

void IOServiceLibEvent::doConnectTimeout(ConnectorChannelContext* ctx)
{
	ctx->unregistWrite();
	ctx->unregistWriteTimer();

	ctx->getApplication()->onConnectError(this, ctx->getChannelId(),
			ctx->getAttachment(), IOApplication::ERR_TIMEOUT);
}

void IOServiceLibEvent::doReadTimeout(ConnectorChannelContext* ctx)
{
	ChannelMessage msg = ctx->popFrontReadMessage();

	handleReadError(ctx, msg.getBuffer(), IOApplication::ERR_TIMEOUT);
}

void IOServiceLibEvent::doWriteTimeout(ConnectorChannelContext* ctx)
{
	ChannelMessage msg = ctx->popFrontWriteMessage();

	handleWriteError(ctx, msg.getBuffer(), IOApplication::ERR_TIMEOUT);
}

void IOServiceLibEvent::prepareForNextRead(ConnectorChannelContext* ctx)
{
	if (!ctx->haveMessageToRead())
	{
		ctx->unregistRead();
		ctx->unregistReadTimer();
	}
	else
	{
		ChannelMessage msg = ctx->frontReadMessage();
		ctx->registRead();
		ctx->registReadTimer(msg.getTimeout());
	}
}

void IOServiceLibEvent::prepareForNextWrite(ConnectorChannelContext* ctx)
{
	if (!ctx->haveMessageToWrite())
	{
		ctx->unregistWrite();
		ctx->unregistWriteTimer();
	}
	else
	{
		ChannelMessage msg = ctx->frontWriteMessage();
		ctx->registWrite();
		ctx->registWriteTimer(msg.getTimeout());
	}
}

void IOServiceLibEvent::handleRead(ConnectorChannelContext* ctx,
		const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& readMsgList)
{
	bool callClosed = false;

	ctx->getApplication()->onMsgRead(this, ctx->getChannelId(),
			ctx->getAttachment(), readMsgList, callClosed);

	if (!callClosed)
	{
		prepareForNextRead(ctx);
	}
}

void IOServiceLibEvent::handleReadError(ConnectorChannelContext* ctx,
		const mxcore::SharedPtr<mxcore::ByteBuffer>& msg, int errorCode)
{
	ctx->unregistRead();
	ctx->unregistReadTimer();

	ctx->popAllReadMessage();

	ctx->getApplication()->onMsgReadError(this, ctx->getChannelId(),
			ctx->getAttachment(), msg, errorCode);
}

void IOServiceLibEvent::handleWrite(ConnectorChannelContext* ctx,
		const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& writtenMsgList)
{
	bool callClosed = false;

	ctx->getApplication()->onMsgWrite(this, ctx->getChannelId(),
			ctx->getAttachment(), writtenMsgList, !ctx->haveMessageToWrite(),
			callClosed);

	if (!callClosed)
	{
		prepareForNextWrite(ctx);
	}
}

void IOServiceLibEvent::handleWriteError(ConnectorChannelContext* ctx,
		const mxcore::SharedPtr<mxcore::ByteBuffer>& msg, int errorCode)
{
	ctx->unregistWrite();
	ctx->unregistWriteTimer();

	ctx->popAllWriteMessage();

	ctx->getApplication()->onMsgWriteError(this, ctx->getChannelId(),
			ctx->getAttachment(), msg, errorCode);
}

ConnectorChannelContext* IOServiceLibEvent::registConnector(
		mxcore::Socket& connector, IOApplication* application,
		const std::string& local, const std::string& remote, void* attachment,
		bool connected)
{
	ChannelId newChannelId = genChannelId(connector.getHandle(), local, remote);

	ConnectorChannelContext* newCtx = new ConnectorChannelContext(this,
			newChannelId, application, attachment);

	newCtx->getConnector().attach(connector.detach(), connected, false);

	channelMap_.insert(std::make_pair(newChannelId, newCtx));

	return newCtx;
}

AcceptorChannelContext* IOServiceLibEvent::registAcceptor(
		mxcore::ServerSocket& acceptor, IOApplication* application,
		const std::string& local, void* attachment)
{
	ChannelId newChannelId = genChannelId(acceptor.getHandle(), local, "");

	AcceptorChannelContext* newCtx = new AcceptorChannelContext(this,
			newChannelId, application, attachment);

	newCtx->getAcceptor().attach(acceptor.detach());

	channelMap_.insert(std::make_pair(newChannelId, newCtx));

	return newCtx;
}

void IOServiceLibEvent::onAccept(int fd, short event, void* arg)
{
	AcceptorChannelContext* ctx = (AcceptorChannelContext*) arg;

	getLogger().trace("IOServiceLibEvent::onAccept(%d, %d, %s)\n", fd, event,
			ctx->getChannelId().toString().c_str());

	ctx->getService()->doAccept(ctx);
}

void IOServiceLibEvent::onRead(int fd, short event, void* arg)
{
	ConnectorChannelContext* ctx = (ConnectorChannelContext*) arg;

	getLogger().trace("OServiceLibEvent::onRead(%d, %d, %s)\n", fd, event,
			ctx->getChannelId().toString().c_str());

	ctx->getService()->doRead(ctx);
}

void IOServiceLibEvent::onWrite(int fd, short event, void* arg)
{
	ConnectorChannelContext* ctx = (ConnectorChannelContext*) arg;

	getLogger().trace("IOServiceLibEvent::onWrite(%d, %d, %s)\n", fd, event,
			ctx->getChannelId().toString().c_str());

	if (ctx->getConnector().isConnecting())
	{
		ctx->getService()->doConnect(ctx);
	}
	else
	{
		ctx->getService()->doWrite(ctx);
	}
}

void IOServiceLibEvent::onWriteTimeout(int fd, short event, void* arg)
{
	ConnectorChannelContext* ctx = (ConnectorChannelContext*) arg;

	getLogger().trace("IOServiceLibEvent::onWriteTimeout(%d, %d, %s)\n", fd,
			event, ctx->getChannelId().toString().c_str());

	if (ctx->getConnector().isConnecting())
	{
		ctx->getService()->doConnectTimeout(ctx);
	}
	else
	{
		ctx->getService()->doWriteTimeout(ctx);
	}
}

void IOServiceLibEvent::onReadTimeout(int fd, short event, void* arg)
{
	ConnectorChannelContext* ctx = (ConnectorChannelContext*) arg;

	getLogger().trace("IOServiceLibEvent::onReadTimeout(%d, %d, %s)\n", fd,
			event, ctx->getChannelId().toString().c_str());

	ctx->getService()->doReadTimeout(ctx);
}

ChannelContext* IOServiceLibEvent::findChannel(const ChannelId& id) const
{
	ChannelMap::const_iterator it = channelMap_.find(id);

	if (it != channelMap_.end())
	{
		return it->second;
	}

	return NULL;
}

void IOServiceLibEvent::closeImpl(ChannelContext* context,
		bool dontCallOnClose, bool closeSock)
{
	context->unregistAll();

	if (!closeSock)
	{
		if (context->isAcceptor())
		{
			((AcceptorChannelContext*) context)->getAcceptor().detach();
		}
		else
		{
			((ConnectorChannelContext*) context)->getConnector().detach();
		}
	}
	if (!dontCallOnClose)
	{

		context->getApplication()->onClose(this, context->getChannelId(),
				context->getAttachment(), context->isAcceptor());
	}

	delete context;
}

ChannelId IOServiceLibEvent::genChannelId(int sock, const std::string& local,
		const std::string& remote)
{
	return ChannelId(sequence_++, sock, local, remote);
}

} // namespace mxasio
