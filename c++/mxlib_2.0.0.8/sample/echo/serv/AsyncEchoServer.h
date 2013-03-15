/*
 * AsyncEchoServer.h
 *
 *  Created on: 2011-3-4
 *      Author: zhaojiangang
 */

#ifndef __MXASIO_ASYNCECHOSERVER_H__
#define __MXASIO_ASYNCECHOSERVER_H__

#include <mxasio/AsyncTcpServer.h>
#include <mxasio/IOApplication.h>

#include <mxcore/SocketAddress.h>

namespace mxasio
{

class AsyncEchoServer: public AsyncTcpApplication
{
public:
	AsyncEchoServer(void);
	virtual ~AsyncEchoServer(void);

	static int main(int argc, char* argv[]);

	void open(const mxcore::SocketAddress& bindAddress)
			throw (mxcore::Exception&);

	void run(void) throw (mxcore::Exception&);

	void setMaxConnection(uint32_t maxConnection);
	void setTimeout(uint32_t recvTimeout, uint32_t sendTimeout);

	virtual void beforeSelect(AsyncTcpServer* tcpServer);

	virtual void afterSelect(AsyncTcpServer* tcpServer);

	virtual void shutdown(AsyncTcpServer* tcpServer);

	virtual void onClose(IOService* service, const ChannelId& id,
			void* attachment, bool isAcceptor);

	virtual void
	* onNewClient(IOService* service, const ChannelId& id, void* attachment,
			const ChannelId& newChannelId, bool& callClosed);

	virtual void
			onMsgRead(
					IOService* service,
					const ChannelId& id,
					void* attachment,
					const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
					bool& callClosed);

	virtual void
	onMsgReadError(IOService* service, const ChannelId& id, void* attachment,
			mxcore::SharedPtr<mxcore::ByteBuffer> message, int errorCode);

	virtual void
			onMsgWrite(
					IOService* service,
					const ChannelId& id,
					void* attachment,
					const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
					bool isHungry, bool& callClosed);

	virtual void
	onMsgWriteError(IOService* service, const ChannelId& id, void* attachment,
			mxcore::SharedPtr<mxcore::ByteBuffer> message, int errorCode);
private:
	AsyncTcpServer tcpServer_;

	uint32_t recvTimeout_;
	uint32_t sendTimeout_;

	uint32_t maxConnection_;
	uint32_t currentConnection_;
};

} // namespace mxasio

#endif /* __MXASIO_ASYNCECHOSERVER_H__ */
