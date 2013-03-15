/*
 *        Name: IOServiceTest.cpp
 *
 *  Created on: 2011-2-23
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <string>
#include <iostream>
#include <mxcore/Verify.h>
#include <mxcore/Number.h>
#include "IOServiceTest.h"
#include <mxasio/IOApplication.h>
#include <mxasio/libevent/IOServiceLibEvent.h>
#include <mxcore/MemRecord.h>

using namespace std;

namespace mxasio
{

static bool canRun = true;
static const int totalClient = 10;
static int closedClient = 0;
static int successedClient = 0;
static int clientErrorCount = 0;
static int writeTimeout = 5000;
static int readTimeout = 5000;

IOServiceTest::IOServiceTest()
{
}

IOServiceTest::~IOServiceTest()
{
}

void IOServiceTest::init(void)
{
}

class EchoSession
{
public:
	EchoSession(const std::string& name, int total) :
		total_(total), name_(name), current_(total), reply_(0)
	{
	}

	~EchoSession(void)
	{
	}

	bool haveNextMessage(void) const
	{
		return current_ > 0;
	}

	string getNextMessage(void)
	{
		MX_ASSERT(haveNextMessage());

		std::string msg(name_);

		msg += ": ";
		msg += mxcore::Integer(current_).toString();
		current_--;
		return msg;
	}
public:
	int total_;
	std::string name_;
	int current_;

	int reply_;
};

class TestServerIOApplication: public IOApplication
{
public:
	virtual void onClose(IOService* service, const ChannelId& id,
			void* attachment, bool isAcceptor)
	{
		cout << "The server channel " << id.toString() << " closed" << endl;
	}

	virtual void* onNewClient(IOService* service, const ChannelId& id,
			void* attachment, const ChannelId& newChannelId, bool& callClosed)
	{
		//		cout << "The server channel " << id.toString() << " new client "
		//				<< newChannelid.toString() << endl;

		mxcore::SharedPtr<mxcore::ByteBuffer> buf = new mxcore::ByteBuffer(256);

		buf->setLimit(1);
		service->recv(newChannelId, buf, readTimeout, true);

		return NULL;
	}

	virtual void onConnect(IOService* service, const ChannelId& id,
			void* attachment)
	{
		// no op
	}

	virtual void onConnectError(IOService* service, const ChannelId& id,
			void* attachment, int errorCode)
	{
		// no op
	}

	virtual void onMsgRead(
			IOService* service,
			const ChannelId& id,
			void* attachment,
			const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
			bool& callClosed)
	{
		send(bufer);
//		for (std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >::const_iterator
//				it = messageList.begin(); it != messageList.end(); it++)
//		{
//			mxcore::SharedPtr<mxcore::ByteBuffer> message = *it;
//
////			if (message->remaining() == 1)
////			{
////				unsigned char size = message->get();
////				message->setLimit(size + 1);
////				service->recv(id, message, readTimeout, true);
////			}
////			else
////			{
////				message->get();
////				std::string msg((const char*) message->getBuffer(),
////						message->remaining());
////				//
////				//				cout << "Client " << id.toString() << " said:[" << msg << "]"
////				//						<< endl;
////
////				message->setPosition(0);
////				service->send(id, message, writeTimeout);
////			}
//		}
	}

	virtual void onMsgReadError(IOService* service, const ChannelId& id,
			void* attachment, mxcore::SharedPtr<mxcore::ByteBuffer> message,
			int errorCode)
	{
		if (-1 != errorCode)
		{
			cout << "****************The server channel " << id.toString()
					<< " read error: [" << errorCode << "]" << endl;
		}
		service->closeChannel(id);
	}

	virtual void onMsgWrite(
			IOService* service,
			const ChannelId& id,
			void* attachment,
			const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
			bool isHungry, bool& callClosed)
	{
		//		cout << "The server channel " << id.toString() << " write ok"
		//				<< endl;
		for (std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >::const_iterator
				it = messageList.begin(); it != messageList.end(); it++)
		{
			mxcore::SharedPtr<mxcore::ByteBuffer> message = *it;
			message->setPosition(0);
			message->setLimit(1);
			service->recv(id, message, readTimeout, true);
		}
	}

	virtual void onMsgWriteError(IOService* service, const ChannelId& id,
			void* attachment, mxcore::SharedPtr<mxcore::ByteBuffer> message,
			int errorCode)
	{
		cout << "*************The server channel " << id.toString()
				<< " write error: [" << errorCode << "]" << endl;

		service->closeChannel(id);
	}
};

class TestIOApplication: public IOApplication
{
public:
	virtual void onClose(IOService* service, const ChannelId& id,
			void* attachment, bool isAcceptor)
	{
		closedClient++;

		if (closedClient >= totalClient)
		{
			canRun = false;
		}
		cout << "The channel " << id.toString() << " closed" << endl;

		EchoSession* session = (EchoSession*) attachment;

		delete session;
	}

	virtual void* onNewClient(IOService* service, const ChannelId& id,
			void* attachment, const ChannelId& newChannelId, bool& callClosed)
	{
		// no op
		return NULL;
	}

	virtual void onConnect(IOService* service, const ChannelId& id,
			void* attachment)
	{
		//		cout << "The channel " << id.toString() << " connected "
		//				<< id.toString() << endl;

		EchoSession* session = (EchoSession*) attachment;

		mxcore::SharedPtr<mxcore::ByteBuffer> message = new mxcore::ByteBuffer(
				256);

		std::string msg = session->getNextMessage();

		BYTE size = msg.size() > 255 ? 255 : (BYTE) msg.size();

		message->put(size);
		message->put(msg.c_str(), size);
		message->flip();

		//		cout << "send: " << msgList.size() << " msg: " << session->current_
		//				<< endl;
		service->send(id, message, writeTimeout);
	}

	virtual void onConnectError(IOService* service, const ChannelId& id,
			void* attachment, int errorCode)
	{
		cout << "The channel " << id.toString() << " connect error: ["
				<< errorCode << "]" << endl;

		cout << "++++++++++++++++++++++++++++++++++++" << endl;
		clientErrorCount++;
		service->closeChannel(id);
	}

	virtual void onMsgRead(
			IOService* service,
			const ChannelId& id,
			void* attachment,
			const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
			bool& callClosed)
	{
		EchoSession* session = (EchoSession*) attachment;

		for (std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >::const_iterator
				it = messageList.begin(); it != messageList.end(); it++)
		{
			mxcore::SharedPtr<mxcore::ByteBuffer> message = *it;

			if (message->remaining() == 1)
			{
				unsigned char size = message->get();
				message->setLimit(size + 1);
				service->recv(id, message, readTimeout, true);
			}
			else
			{
				message->get();
				std::string msg((const char*) message->getBuffer(),
						message->remaining());

				if (session->haveNextMessage())
				{
					std::string msg = session->getNextMessage();

					BYTE size = msg.size() > 255 ? 255 : (BYTE) msg.size();

					message->clear();
					message->put(size);
					message->put(msg.c_str(), size);
					message->flip();

					service->send(id, message, writeTimeout);
				}
				else
				{
					service->closeChannel(id);
					return;
				}
				//			cout << "Server " << id.toString() << " reply:[" << msg << "]"
				//					<< endl;
			}
		}

	}

	virtual void onMsgReadError(IOService* service, const ChannelId& id,
			void* attachment, mxcore::SharedPtr<mxcore::ByteBuffer> message,
			int errorCode)
	{
		cout << "The channel " << id.toString() << " read error: ["
				<< errorCode << "]" << endl;
		cout << "++++++++++++++++++++++++++++++++++++" << endl;
		clientErrorCount++;
		service->closeChannel(id);
	}

	virtual void onMsgWrite(
			IOService* service,
			const ChannelId& id,
			void* attachment,
			const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& messageList,
			bool isHungry, bool& callClosed)
	{
		//		cout << "The channel " << id.toString() << " write ok" << endl;

		for (std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >::const_iterator
				it = messageList.begin(); it != messageList.end(); it++)
		{
			mxcore::SharedPtr<mxcore::ByteBuffer> message = *it;
			message->setPosition(0);
			service->recv(id, message, readTimeout, true);
		}
	}

	virtual void onMsgWriteError(IOService* service, const ChannelId& id,
			void* attachment, mxcore::SharedPtr<mxcore::ByteBuffer> message,
			int errorCode)
	{
		cout << "The channel " << id.toString() << " write error: ["
				<< errorCode << "]" << endl;
		cout << "++++++++++++++++++++++++++++++++++++" << endl;
		clientErrorCount++;
		service->closeChannel(id);
	}
};

void IOServiceTest::perform(void)
{
	std::cout << "IOServiceTest::perform ..." << std::endl;
	TestIOApplication echoClient;
	TestServerIOApplication echoServer;

	mxcore::SocketAddress local = mxcore::SocketAddress::loopback(AF_INET,
			20000);

	clientErrorCount = 0;

	time_t start = time(NULL);

	try
	{
		std::auto_ptr<IOService> service(IOService::createInstance());
		service->listen(&echoServer, local, totalClient, true, NULL);

		int i = 0;
		int count = 0;
		while (i < totalClient && count < 1)
		{
			ChannelId id = service->connect(
					&echoClient,
					local,
					10000,
					new EchoSession(
							std::string("james_")
									+ mxcore::Integer(i).toString(), 5));

			//			cout << i << " Client channel id: " << id.toString() << endl;

			i++;
			count++;
		}
		while (canRun)
		{
			service->select();

			count = 0;

			while (i < totalClient && count < 1)
			{
				ChannelId id = service->connect(
						&echoClient,
						local,
						10000,
						new EchoSession(
								std::string("james_")
										+ mxcore::Integer(i).toString(), 5));

				cout << i << " Client channel id: " << id.toString() << endl;

				i++;
				count++;
			}
		}
		service->close();

		MX_VERIFY(clientErrorCount == 0);
		MX_VERIFY(successedClient = totalClient);
		cout << "errorClient:" << clientErrorCount << ", successedClient:"
				<< successedClient << ", used:" << time(NULL) - start
				<< " seconds" << endl;
	} catch (mxcore::Exception& e)
	{
		cout << "Error: " << e.getMessage() << ":" << e.getLineNumber() << endl;
	}
}

void IOServiceTest::cleanup(void)
{
}

}
