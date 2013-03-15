/*
 * HttpServerImpl.h
 *
 *  Created on: 2011-3-1
 *      Author: zhaojiangang
 */

#ifndef __MXHTTP_IMPL_HTTPSERVERIMPL_H__
#define __MXHTTP_IMPL_HTTPSERVERIMPL_H__

#include <mxcore/MutexLock.h>
#include <mxcore/LoggerFactory.h>

#include <mxasio/IOService.h>
#include <mxasio/AsyncTcpServer.h>
#include <mxasio/AsyncTcpApplication.h>

#include <ext/hash_set>
#include <ext/hash_map>
#include <mxhttp/HttpServer.h>
#include <mxhttp/HttpResponser.h>
#include <mxhttp/HttpListening.h>
#include <mxhttp/impl/HttpServerConnection.h>

namespace mxhttp
{

class HttpServerImpl: public HttpServer,
		public mxasio::AsyncTcpApplication,
		public HttpResponser
{
	static mxcore::Logger& getLogger(void);
public:
	HttpServerImpl(const HttpConfig& config);

	virtual ~HttpServerImpl(void);

	virtual void open(void) throw (mxcore::Exception&);

	virtual void reopen(void) throw (mxcore::Exception&);

	virtual void shutdown(void) throw (mxcore::Exception&);

	virtual void run(void) throw (mxcore::Exception&);

	virtual uint32_t getConnectionCount(void) const;

	virtual bool addVirtualServer(
			mxcore::SharedPtr<HttpVirtualServer> virtualServer,
			const std::list<mxcore::SocketAddress>* bindList);

	virtual mxasio::IOService* getIOService(void) const;

	virtual bool response(mxcore::SharedPtr<HttpRequest> request,
			mxcore::SharedPtr<HttpResponse> response);

	virtual bool asyncResponse(mxcore::SharedPtr<HttpRequest> request,
			mxcore::SharedPtr<HttpResponse> response);

	virtual void beforeSelect(mxasio::AsyncTcpServer* tcpServer);

	virtual void afterSelect(mxasio::AsyncTcpServer* tcpServer);

	virtual void shutdown(mxasio::AsyncTcpServer* tcpServer);

	virtual void onClose(mxasio::IOService* service,
			const mxasio::ChannelId& id, void* attachment, bool isAcceptor);

	virtual void
	* onNewClient(mxasio::IOService* service, const mxasio::ChannelId& id,
			void* attachment, const mxasio::ChannelId& newChannelId,
			bool& callClosed);

	virtual void
			onMsgRead(
					mxasio::IOService* service,
					const mxasio::ChannelId& id,
					void* attachment,
					const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
					bool& callClosed);

	virtual void
	onMsgReadError(mxasio::IOService* service, const mxasio::ChannelId& id,
			void* attachment, mxcore::SharedPtr<mxcore::ByteBuffer> message,
			int errorCode);

	virtual void
			onMsgWrite(
					mxasio::IOService* service,
					const mxasio::ChannelId& id,
					void* attachment,
					const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
					bool isHungry, bool& callClosed);

	virtual void
	onMsgWriteError(mxasio::IOService* service, const mxasio::ChannelId& id,
			void* attachment, mxcore::SharedPtr<mxcore::ByteBuffer> message,
			int errorCode);
private:
	HttpServerConnection* findConnection(const HttpConnectionId& id) const;

	void sendResponse(HttpServerConnection* connection);

	HttpMsgParser* createParser(void) const;

	bool
	checkRequest(HttpServerConnection* connection,
			std::pair<int, std::string>& status) const;
	void handleRequest(HttpServerConnection* connection);
	bool handleError(HttpServerConnection* connection,
			const std::pair<int, std::string>& status, bool canClosed = true);

	typedef std::map<std::string, HttpListening*> ListeningMap;

	HttpListening
	* findDefaultListening(const mxcore::SocketAddress& address) const;

	HttpListening* findOrCreateListening(ListeningMap& listenings,
			const mxcore::SocketAddress& address);

	void registerListenings(const ListeningMap& listenings);
	void registerListening(HttpListening* listening);
private:
	mxasio::AsyncTcpServer* tcpServer_;

	typedef __gnu_cxx ::hash_map<HttpConnectionId, HttpServerConnection*,
			HttpConnectionIdHash, HttpConnectionIdEqual>
			HttpServerConnectionMap;

	HttpServerConnectionMap connectionMap_;
	mutable mxcore::AutoMutexLock connectionLock_;

	HttpConfig config_;
	TempFileNameGeneratorSimple tempFilenameGen_;

	ListeningMap listenings_;
	ListeningMap defaultListenings_;

	std::set<HttpListening*> allListenings_;

	typedef __gnu_cxx ::hash_set<HttpConnectionId, HttpConnectionIdHash,
			HttpConnectionIdEqual> ConnectionIdSet;

	ConnectionIdSet asyncResponses_;

	std::list<mxcore::LoggerFactory::LoggerConfig> extLoggers_;
};

} // namespace mxhttp

#endif /* __MXHTTP_IMPL_HTTPSERVERIMPL_H__ */
