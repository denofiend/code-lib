/*
 * HttpServerImpl.cpp
 *
 *  Created on: 2011-3-1
 *      Author: zhaojiangang
 */

#include <iostream>
#include <mxcore/SystemOps.h>
#include <mxhttp/HttpUtil.h>
#include <mxhttp/impl/HttpServerImpl.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

mxcore::Logger& HttpServerImpl::getLogger(void)
{
	return mxcore::LoggerFactory::getInstance().getLogger("mxhttp");
}

HttpServerImpl::HttpServerImpl(const HttpConfig& config) :
	tcpServer_(new mxasio::AsyncTcpServer), config_(config),
			tempFilenameGen_(config.tempRepository_)
{
	for (std::list<mxcore::SocketAddress>::const_iterator it =
			config_.listenList_.begin(); it != config_.listenList_.end(); it++)
	{
		HttpListening* listening = new HttpListening(*it);
		defaultListenings_.insert(std::make_pair((*it).toString(), listening));
		allListenings_.insert(listening);
	}
}

HttpServerImpl::~HttpServerImpl(void)
{
	delete tcpServer_;

	for (std::set<HttpListening*>::iterator it = allListenings_.begin(); it
			!= allListenings_.end(); it++)
	{
		delete *it;
	}
	allListenings_.clear();
}

void HttpServerImpl::open(void) throw (mxcore::Exception&)
{
	MX_ASSERT(tcpServer_->getState() == mxasio::AsyncTcpServer::eClosed);

	uint32_t uid = mxos::getUid(config_.username_.c_str());

	if ((uint32_t) - 1 == uid)
	{
		THROW2(mxcore::Exception,
				std::string("Unknown username: [") + config_.username_ + "]");
	}

	for (std::list<mxcore::LoggerFactory::LoggerConfig>::const_iterator it =
			config_.loggerList_.begin(); it != config_.loggerList_.end(); it++)
	{
		try
		{
			const mxcore::LoggerFactory::LoggerConfig& logConfig = *it;
			mxcore::LoggerFactory::getInstance().createLogger(logConfig);
		} catch (mxcore::IOException& e)
		{
			throw e;
		}
	}

	for (std::list<mxcore::LoggerFactory::LoggerConfig>::const_iterator it =
			extLoggers_.begin(); it != extLoggers_.end(); it++)
	{
		try
		{
			const mxcore::LoggerFactory::LoggerConfig& logConfig = *it;
			mxcore::LoggerFactory::getInstance().createLogger(logConfig);
		} catch (mxcore::IOException& e)
		{
			throw e;
		}
	}

	mxasio::AsyncTcpServer::Config cfg(uid, config_.subProcessNum_);
	tcpServer_->setConfig(cfg);

	registerListenings( defaultListenings_);
	registerListenings( listenings_);

	tcpServer_->open();
}

void HttpServerImpl::reopen(void) throw (mxcore::Exception&)
{
	MX_ASSERT(tcpServer_->getState() == mxasio::AsyncTcpServer::eOpen);

	mxcore::LoggerFactory::getInstance().reopen();
	tcpServer_->reopen();
}

void HttpServerImpl::run(void) throw (mxcore::Exception&)
{
	MX_ASSERT(
			mxasio::AsyncTcpServer::eOpen == tcpServer_->getState()
					|| mxasio::AsyncTcpServer::eReopen
							== tcpServer_->getState());

	tcpServer_->run();
}

uint32_t HttpServerImpl::getConnectionCount(void) const
{
	mxcore::Lock::ScopeLock sl(connectionLock_);
	return (uint32_t) connectionMap_.size();
}

bool HttpServerImpl::addVirtualServer(
		mxcore::SharedPtr<HttpVirtualServer> virtualServer,
		const std::list<mxcore::SocketAddress>* bindList)
{
	MX_ASSERT(!virtualServer.isNull());

	virtualServer->httpServer_ = this;

	bool ret = false;

	if (NULL == bindList || bindList->empty())
	{
		for (ListeningMap::iterator it = defaultListenings_.begin(); it
				!= defaultListenings_.end(); it++)
		{
			if (it->second->addVirtualServer(virtualServer) && !ret)
			{
				ret = true;
			}
		}
	}
	else
	{
		for (std::list<mxcore::SocketAddress>::const_iterator it =
				bindList->begin(); it != bindList->end(); it++)
		{
			HttpListening* listening = findDefaultListening(*it);

			if (NULL == listening)
			{
				listening = findOrCreateListening(listenings_, *it);
			}

			if (listening->addVirtualServer(virtualServer) && !ret)
			{
				ret = true;
			}
		}
	}

	if (ret)
	{
		if (!virtualServer->getLoggerConfig().name_.empty())
		{
			extLoggers_.push_back(virtualServer->getLoggerConfig());
		}
	}
	return ret;
}

mxasio::IOService* HttpServerImpl::getIOService(void) const
{
	return tcpServer_->getIOService();
}

void HttpServerImpl::shutdown(void) throw (mxcore::Exception&)
{
	getLogger().info("The http server shutdown\n");
	tcpServer_->shutdown();
}

void HttpServerImpl::beforeSelect(mxasio::AsyncTcpServer* tcpServer)
{
	mxcore::Lock::ScopeLock sl(connectionLock_);

	for (ConnectionIdSet::iterator it = asyncResponses_.begin(); it
			!= asyncResponses_.end(); it++)
	{
		const HttpConnectionId& id = *it;

		HttpServerConnection* connection = findConnection(id);

		if (NULL != connection)
		{
			sendResponse(connection);
		}
	}

	asyncResponses_.clear();
}

void HttpServerImpl::afterSelect(mxasio::AsyncTcpServer* tcpServer)
{
}

void HttpServerImpl::shutdown(mxasio::AsyncTcpServer* tcpServer)
{
}

bool HttpServerImpl::response(mxcore::SharedPtr<HttpRequest> request,
		mxcore::SharedPtr<HttpResponse> response)
{
	HttpServerConnection* connection = findConnection(
			request->getConnectionId());

	if (connection == NULL)
	{
		return false;
	}
	MX_ASSERT(connection->response_.isNull());

	connection->response_ = response;
	sendResponse(connection);
	return true;
}

bool HttpServerImpl::asyncResponse(mxcore::SharedPtr<HttpRequest> request,
		mxcore::SharedPtr<HttpResponse> response)
{
	mxcore::Lock::ScopeLock sl(connectionLock_);

	HttpServerConnection* connection = findConnection(
			request->getConnectionId());
	if (connection == NULL)
	{
		return false;
	}

	MX_ASSERT(connection->response_.isNull());
	connection->response_ = response;

	asyncResponses_.insert(request->getConnectionId());
	tcpServer_->getIOService()->wakeup();
	return true;
}

void HttpServerImpl::onClose(mxasio::IOService* service,
		const mxasio::ChannelId& id, void* attachment, bool isAcceptor)
{
	getLogger().debug("Connection: [%s] closed\n", id.toString().c_str());

	if (!isAcceptor)
	{
		{
			mxcore::Lock::ScopeLock sl(connectionLock_);
			connectionMap_.erase(id);
		}
		HttpServerConnection* connection = (HttpServerConnection*) attachment;
		delete connection;
	}
}

void * HttpServerImpl::onNewClient(mxasio::IOService* service,
		const mxasio::ChannelId& id, void* attachment,
		const mxasio::ChannelId& newChannelId, bool& callClosed)
{
	if (getConnectionCount() >= config_.maxConnectionPerProcess_)
	{
		callClosed = true;
		service->closeChannel(newChannelId);
		getLogger().debug("Reject Connection: [%s] on listen: [%s]\n",
				newChannelId.toString().c_str(), id.toString().c_str());
		return NULL;
	}

	HttpServerConnection* connection = new HttpServerConnection(newChannelId,
			(HttpListening*) attachment);

	getLogger().debug("Accept Connection: [%s] on listen: [%s]\n",
			newChannelId.toString().c_str(), id.toString().c_str());

	connection->parser_ = createParser();
	connection->readBuf_ = new mxcore::ByteBuffer(config_.bufferSize_);

	{
		mxcore::Lock::ScopeLock sl(connectionLock_);
		connectionMap_.insert(std::make_pair(newChannelId, connection));
	}
	service->recv(newChannelId, connection->readBuf_, config_.recvTimeout_,
			false);
	return connection;
}

static void adjustBuffer(mxcore::SharedPtr<mxcore::ByteBuffer>& buffer)
{
	uint32_t remaining = buffer->remaining();

	if (remaining > 0 && remaining < buffer->getCapacity())
	{
		::memmove(buffer->getOrgBuffer(), buffer->getBuffer(), remaining);
	}
	buffer->setLimit(buffer->getCapacity());
	buffer->setPosition(remaining);
}

void HttpServerImpl::onMsgRead(mxasio::IOService* service,
		const mxasio::ChannelId& id, void* attachment,
		const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
		bool& callClosed)
{
	MX_ASSERT(messageList.size() == 1);

	HttpServerConnection* connection = (HttpServerConnection*) attachment;
	mxcore::SharedPtr < mxcore::ByteBuffer > message = messageList.front();

	try
	{
		int parsedLen = connection->parser_->append(message->getBuffer(),
				message->remaining());

		if (NULL != connection->parser_->getMessage())
		{
			connection->parser_->getMessage()->setConnectionId(id);
		}

		message->setPosition(message->getPosition() + parsedLen);

		adjustBuffer( message);

		if (message->remaining() == 0)
		{
			//buffer 满了
			if (!handleError(connection, HttpConstants::STATUS_BAD_REQUEST))
			{
				callClosed = true;
				service->closeChannel(id);
				return;
			}
		}

		std::pair<int, std::string> status = HttpConstants::STATUS_OK;

		if (!checkRequest(connection, status))
		{
			handleError(connection, status, connection->parser_->isComplated());

			if (!connection->parser_->isComplated())
			{
				callClosed = true;
				service->closeChannel(id);
			}
			return;
		}

		if (connection->parser_->isComplated())
		{
			HttpRequest* request =
					(HttpRequest*) connection->parser_->detachMessage();

			request->setClientIp(
					id.getRemote().substr(0, id.getRemote().find(':')));
			connection->request_ = request;

			handleRequest(connection);
		}
		else
		{
			service ->recv(id, message, config_.recvTimeout_, false);
		}
	} catch (HttpException& e)
	{
		if (!handleError(connection, HttpConstants::STATUS_BAD_REQUEST))
		{
			callClosed = true;
			service->closeChannel(id);
		}
	} catch (mxcore::Exception& e)
	{
		if (!handleError(connection, HttpConstants::STATUS_SERVER_ERROR))
		{
			callClosed = true;
			service->closeChannel(id);
		}
	}
}

void HttpServerImpl::onMsgReadError(mxasio::IOService* service,
		const mxasio::ChannelId& id, void* attachment,
		mxcore::SharedPtr<mxcore::ByteBuffer> message, int errorCode)
{
	HttpServerConnection* connection = (HttpServerConnection*) attachment;

	if (IOApplication::ERR_EOF == errorCode)
	{
		if (getLogger().isDebugEnable())
		{
			getLogger().debug("Connection: [%s] eof\n", id.toString().c_str());
		}
	}
	else if (IOApplication::ERR_TIMEOUT == errorCode)
	{
		if (getLogger().isDebugEnable())
		{
			getLogger().debug("Connection: [%s] read timeout\n",
					id.toString().c_str());
		}
	}
	else
	{
		if (getLogger().isErrorEnable())
		{
			getLogger().error2(errorCode, "Connection: [%s] read error\n",
					id.toString().c_str());
		}
	}
	// 如果没有数据可以发送了，或者有数据可以发送并且不是超时而且不是对方关闭了写
	if (connection->response_.isNull() || (errorCode
			!= mxasio::IOApplication::ERR_EOF && errorCode
			!= mxasio::IOApplication::ERR_TIMEOUT))
	{
		//  接收数据超时，或对方已经关闭了写，并且服务端已经没有数据需要发送了
		service->closeChannel(id);
	}
}

void HttpServerImpl::onMsgWrite(mxasio::IOService* service,
		const mxasio::ChannelId& id, void* attachment,
		const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
		bool isHungry, bool& callClosed)
{
	HttpServerConnection* connection = (HttpServerConnection*) attachment;

	if (!isHungry)
	{
		return;
	}

	try
	{
		HttpSimpleBody* body = connection->response_.isNull() ? NULL
				: (HttpSimpleBody*) connection->response_->getBody();

		if (NULL == body || body->available() <= 0)
		{
			if ((!config_.forceKeepAvlie_
					&& !connection->request_->isKeepAlive())
					|| config_.keepAliveTimeout_ <= 0)
			{
				connection->canClosed_ = true;
			}

			// 没有数据可以发送, 并且可以退出
			if (connection->canClosed_)
			{
				callClosed = true;
				service->closeChannel(id);
				return;
			}

			connection->request_ = NULL;
			connection->response_ = NULL;
			connection->checkedUriAndHeaders_ = false;
			connection->parser_->clear();

			// 接收新的request
			service->recv(id, connection->readBuf_, config_.keepAliveTimeout_,
					false);
		}
		else
		{
			for (std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >::const_iterator
					it = messageList.begin(); body->available() > 0 && it
					!= messageList.end(); it++)
			{
				mxcore::SharedPtr < mxcore::ByteBuffer > message = *it;
				message->clear();

				int r = body->read(message->getBuffer(), message->remaining());
				MX_ASSERT(r > 0);
				message->setPosition(message->getPosition() + r);
				message->flip();
				service->send(id, message, config_.sendTimeout_);
			}
		}
	} catch (mxcore::IOException& e)
	{
		if (getLogger().isErrorEnable())
		{
			getLogger().error1(
					"Connection: [%s]: failed to read response body: %s\n",
					id.toString().c_str(), e.getMessage().c_str());
		}

		callClosed = true;
		service->closeChannel(id);
	}
}

void HttpServerImpl::onMsgWriteError(mxasio::IOService* service,
		const mxasio::ChannelId& id, void* attachment,
		mxcore::SharedPtr<mxcore::ByteBuffer> message, int errorCode)
{
	if (getLogger().isErrorEnable())
	{
		getLogger().error2(errorCode,
				"Connection: [%s]: failed to write response\n",
				id.toString().c_str());
	}

	service->closeChannel(id);
}

HttpServerConnection* HttpServerImpl::findConnection(const HttpConnectionId& id) const
{
	HttpServerConnectionMap::const_iterator it = connectionMap_.find(id);

	if (it != connectionMap_.end())
	{
		return it->second;
	}

	return NULL;
}

void HttpServerImpl::sendResponse(HttpServerConnection* connection)
{
	getLogger().info("Connection: [%s], %s %s %d\n",
			connection->id_.toString().c_str(),
			connection->request_->getMethod().c_str(),
			connection->request_->getUri().c_str(),
			connection->response_->getStatusCode());

	MX_ASSERT(!connection->response_.isNull());
	std::string responseHeader;
	HttpUtil::encodeResponseHeaders(*connection->response_, responseHeader);

	uint32_t size = (uint32_t) responseHeader.size();

	if (size < 4096)
	{
		size = 4096;
	}

	mxcore::SharedPtr < mxcore::ByteBuffer > message = new mxcore::ByteBuffer(
			size);

	message->put(responseHeader.c_str(), (uint32_t) responseHeader.size());

	HttpSimpleBody* body = (HttpSimpleBody*) connection->response_->getBody();

	if (NULL != body && body->available() > 0 && body->getContentLength()
			<= message->remaining())
	{
		body->read(message->getBuffer(), body->getContentLength());
		message->setPosition(message->getPosition() + body->getContentLength());
	}

	message->flip();
	tcpServer_->getIOService()->send(connection->id_, message,
			config_.sendTimeout_);
}

HttpMsgParser* HttpServerImpl::createParser(void) const
{
	return new HttpMsgParser(config_.maxContentLength_,
			config_.cachedLimitSize_,
			(TempFileNameGenerator*) &tempFilenameGen_);
}

bool HttpServerImpl::checkRequest(HttpServerConnection* connection,
		std::pair<int, std::string>& status) const
{
	if (connection->checkedUriAndHeaders_)
	{
		return true;
	}

	if (connection->parser_->getMessage() == NULL)
	{
		return true;
	}

	connection->checkedUriAndHeaders_ = true;

	HttpRequest* request = connection->parser_->getRequest();

	if (NULL == request)
	{
		status = HttpConstants::STATUS_BAD_REQUEST;
		return false;
	}

	std::pair < mxcore::SharedPtr<HttpVirtualServer>, mxcore::SharedPtr<
			HttpServlet> > result;
	if (!connection->listening_->findServlet(*request, result))
	{
		status = HttpConstants::STATUS_NOT_FOUND;
		return false;
	}

	return true;
}

void HttpServerImpl::handleRequest(HttpServerConnection* connection)
{
	std::pair < mxcore::SharedPtr<HttpVirtualServer>, mxcore::SharedPtr<
			HttpServlet> > result;

	if (connection->listening_->findServlet(*connection->request_, result))
	{
		try
		{
			result.second->init();
			result.second->service(result.first, connection->request_, this);
		} catch (mxcore::Exception& e)
		{
			getLogger().trace(
					"Handle request: %s %s servlet init failed: %s %s %s:%d\n",
					connection->request_->getMethod().c_str(),
					connection->request_->getUri().c_str(), e.name().c_str(),
					e.getMessage().c_str(), e.getFilename().c_str(),
					e.getLineNumber());

			handleError(connection, HttpConstants::STATUS_SERVER_ERROR, false);
		}
	}
	else
	{
		handleError(connection, HttpConstants::STATUS_NOT_FOUND, false);
	}
}

bool HttpServerImpl::handleError(HttpServerConnection* connection,
		const std::pair<int, std::string>& status, bool canClosed)
{
	HttpRequest* request = connection->parser_->getRequest();

	if (NULL != request)
	{
		connection->request_ = request;
		connection->parser_->detachMessage();
	}

	if (!connection->request_.isNull())
	{
		connection->canClosed_ = canClosed;

		connection->response_ = HttpUtil::makeResponse(*connection->request_,
				status, NULL);

		sendResponse(connection);
		return true;
	}

	return false;
}

HttpListening* HttpServerImpl::findDefaultListening(
		const mxcore::SocketAddress& address) const
{
	std::string key = address.toString();

	ListeningMap::const_iterator it = defaultListenings_.find(key);
	if (it != defaultListenings_.end())
	{
		return it->second;
	}
	return NULL;
}

HttpListening* HttpServerImpl::findOrCreateListening(ListeningMap& listenings,
		const mxcore::SocketAddress& address)
{
	HttpListening* listening = NULL;

	std::string key = address.toString();
	ListeningMap::iterator it = listenings.find(key);
	if (it != listenings.end())
	{
		listening = it->second;
	}
	else
	{
		listening = new HttpListening(address);
		listenings.insert(std::make_pair(key, listening));
		allListenings_.insert(listening);
	}
	return listening;
}

void HttpServerImpl::registerListenings(const ListeningMap& listenings)
{
	for (ListeningMap::const_iterator it = listenings.begin(); it
			!= listenings.end(); it++)
	{
		registerListening(it->second);
	}
}

void HttpServerImpl::registerListening(HttpListening* listening)
{
	tcpServer_->registerTcpApplication(listening->getBindAddress(), 2000, true,
			this, listening);
}

} // namespace mxhttp
