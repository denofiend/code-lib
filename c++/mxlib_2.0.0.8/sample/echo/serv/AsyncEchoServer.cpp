/*
 * AsyncEchoServer.cpp
 *
 *  Created on: 2011-3-4
 *      Author: zhaojiangang
 */

#include <iostream>
#include "AsyncEchoServer.h"
#include <mxcore/SystemOps.h>
#include <mxcore/MemRecord.h>

namespace mxasio
{

AsyncEchoServer::AsyncEchoServer(void) :
	recvTimeout_(0), sendTimeout_(0), maxConnection_(0), currentConnection_(0)
{
}

AsyncEchoServer::~AsyncEchoServer(void)
{
}

static void printUsage(char* argv[])
{
	std::cout << argv[0] << " <#port> <#maxConnection>" << std::endl;
}

int AsyncEchoServer::main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printUsage(argv);
		return -1;
	}

	try
	{
		mxcore::UShort port = mxcore::UShort::fromString(argv[1]);
		mxcore::UInteger maxConnection = mxcore::UInteger::fromString(argv[2]);

		mxcore::SocketAddress bindAddress;
		bindAddress.setPort(port);

		{
			mxasio::AsyncEchoServer server;
			server.open(bindAddress);
			server.run();
		}
	} catch (mxcore::NumberFormatException& e)
	{
		printUsage(argv);
	} catch (mxcore::Exception& e)
	{
		std::cout << e.name() << ", " << e.getMessage() << ", "
				<< e.getFilename() << ":" << e.getLineNumber() << std::endl;
	}
}

void AsyncEchoServer::open(const mxcore::SocketAddress& bindAddress)
		throw (mxcore::Exception&)
{
	tcpServer_.registerTcpApplication(bindAddress, 1000, true, this, NULL);
	tcpServer_.open();
}

void AsyncEchoServer::run(void) throw (mxcore::Exception&)
{
	tcpServer_.run();
}

void AsyncEchoServer::setMaxConnection(uint32_t maxConnection)
{
	maxConnection_ = maxConnection;
}

void AsyncEchoServer::setTimeout(uint32_t recvTimeout, uint32_t sendTimeout)
{
	recvTimeout_ = recvTimeout;
	sendTimeout_ = sendTimeout;
}

void AsyncEchoServer::beforeSelect(AsyncTcpServer* tcpServer)
{
}

void AsyncEchoServer::afterSelect(AsyncTcpServer* tcpServer)
{
}

void AsyncEchoServer::shutdown(AsyncTcpServer* tcpServer)
{
}

void AsyncEchoServer::onClose(IOService* service, const ChannelId& id,
		void* attachment, bool isAcceptor)
{
	currentConnection_--;
}

void * AsyncEchoServer::onNewClient(IOService* service, const ChannelId& id,
		void* attachment, const ChannelId& newChannelId, bool& callClosed)
{
	if (maxConnection_ > 0 && currentConnection_ + 1 > maxConnection_)
	{
		callClosed = true;
		service->closeChannel(id);
	}
	else
	{
		currentConnection_++;
		mxcore::SharedPtr<mxcore::ByteBuffer> buffer = new mxcore::ByteBuffer(
				4096);

		service->recv(newChannelId, buffer, recvTimeout_);

	}
	return NULL;
}

void AsyncEchoServer::onMsgRead(IOService* service, const ChannelId& id,
		void* attachment,
		const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
		bool& callClosed)
{
	MX_ASSERT(messageList.size() == 1);

	mxcore::SharedPtr<mxcore::ByteBuffer> buffer = messageList.front();

	std::string msg((const char*) buffer->getBuffer(), buffer->remaining());

	service->send(id, buffer, sendTimeout_);
}

void AsyncEchoServer::onMsgReadError(IOService* service, const ChannelId& id,
		void* attachment, mxcore::SharedPtr<mxcore::ByteBuffer> message,
		int errorCode)
{
	if (errorCode != -1)
	{
		std::cout << "Channel: " << id.toString() << " read error: "
				<< errorCode << std::endl;
	}
	service->closeChannel(id);
}

void AsyncEchoServer::onMsgWrite(IOService* service, const ChannelId& id,
		void* attachment,
		const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
		bool isHungry, bool& callClosed)
{
	MX_ASSERT(messageList.size() == 1);

	mxcore::SharedPtr<mxcore::ByteBuffer> buffer = messageList.front();

	buffer->clear();
	service->recv(id, buffer, recvTimeout_);
}

void AsyncEchoServer::onMsgWriteError(IOService* service, const ChannelId& id,
		void* attachment, mxcore::SharedPtr<mxcore::ByteBuffer> message,
		int errorCode)
{
	service->closeChannel(id);
	std::cout << "Channel: " << id.toString() << " write error: " << errorCode
			<< std::endl;
}

} // namespace mxasio

//int main(int argc, char* argv[])
//{
//	mxasio::AsyncEchoServer::main(argc, argv);
//}
