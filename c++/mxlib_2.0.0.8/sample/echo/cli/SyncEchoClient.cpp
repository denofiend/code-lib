/*
 * SyncEchoClient.cpp
 *
 *  Created on: 2011-3-4
 *      Author: zhaojiangang
 */

#include <iostream>
#include <mxcore/SystemOps.h>
#include <mxcore/Number.h>
#include "SyncEchoClient.h"
#include <mxcore/MemRecord.h>

namespace mxasio
{

SyncEchoClient::SyncEchoClient(void)
{
}

SyncEchoClient::~SyncEchoClient(void)
{
	close();
}

static void printUsage(char* argv[])
{
	std::cout << argv[0] << " <#remoteIp> <port> <#processNum> <loop> <times>"
			<< std::endl;
}

static void processRun(uint32_t index, const mxcore::SocketAddress& remote,
		uint32_t loopN, uint32_t times)
{
	SyncEchoClient client;
	uint32_t successed = 0;
	uint32_t total = loopN * times;

	try
	{
		std::string message("this is ");
		message += mxcore::ULong(mxos::getPid()).toString();
		message += " message";

		for (uint32_t i = 0; i < loopN; i++)
		{
			client.connect(remote);
			for (uint32_t j = 0; j < times; j++)
			{
				if (client.sendAndRecv(message))
				{
					successed++;
				}
				else
				{
					std::cout << "----" << std::endl;
				}
			}
			client.close();
		}
	} catch (mxcore::Exception& e)
	{
		std::cout << "PID: " << mxos::getPid() << ", " << e.name() << ", "
				<< e.getMessage() << ", " << e.getFilename() << ":"
				<< e.getLineNumber() << std::endl;
	}

	std::cout << "PID: " << mxos::getPid() << ", total: " << total
			<< ", successed: " << successed << std::endl;
}

int SyncEchoClient::main(int argc, char* argv[])
{
	if (argc < 6)
	{
		printUsage(argv);
		return 0;
	}

	uint16_t port = 0;
	uint32_t processNum = 0;
	uint32_t loopN = 0;
	uint32_t times = 0;

	try
	{
		port = mxcore::UShort::fromString(argv[2]);
		processNum = mxcore::UShort::fromString(argv[3]);
		loopN = mxcore::UInteger::fromString(argv[4]);
		times = mxcore::UInteger::fromString(argv[5]);
	} catch (mxcore::NumberFormatException& e)
	{
		printUsage(argv);
		return 0;
	}

	time_t startTime = time(NULL);
	try
	{
		mxcore::SocketAddress remote;
		remote.set(argv[1], AF_INET, port);
#if defined(WIN32)
		processRun(0, remote, loopN, times);
#else
		for (uint32_t i = 0; i < processNum; i++)
		{
			long pid = fork();

			if (pid == 0)
			{
				processRun(i, remote, loopN, times);
				exit(0);
			}
			else if (pid < 0)
			{
				std::cout << "fork " << i << " failed" << std::endl;
			}
		}
#endif
	} catch (mxcore::Exception& e)
	{
		std::cout << e.name() << ", " << e.getMessage() << ", "
				<< e.getFilename() << ":" << e.getLineNumber() << std::endl;
	}

	while (wait(NULL) > 0)
		/* now parent waits for all children */
		;

	time_t endTime = time(NULL);
	std::cout << argv[0] << " " << argv[1] << " " << argv[2] << " " << argv[3]
			<< " " << argv[4] << " " << argv[5] << " used: " << (endTime
			- startTime) << " seconds" << std::endl;

	return 0;
}

void SyncEchoClient::connect(const mxcore::SocketAddress& remote)
		throw (mxcore::IOException&)
{
	close();

	socket_.connect(remote);
}

static bool readN(mxcore::Socket& sock, std::string& message, int n)
		throw (mxcore::IOException&)
{
	char buffer[1024];
	while ((int) message.size() < n)
	{
		int toRead = n - message.size();
		if (toRead > 1024)
		{
			toRead = 1024;
		}

		int r = sock.readN(buffer, toRead);

		if (toRead != r)
		{
			std::cout << "toRead: " << toRead << ", read: " << r
					<< ", message: [" << message << "]" << std::endl;
			return false;
		}
		message.append(buffer, toRead);
	}

	return true;
}

bool SyncEchoClient::sendAndRecv(const std::string& message)
		throw (mxcore::IOException&)
{
	MX_ASSERT(!message.empty());
	std::string recvMsg;
	socket_.writeN(message.c_str(), message.size());
	bool ret = readN(socket_, recvMsg, (int) message.size());
	return ret && (message == recvMsg);
}

void SyncEchoClient::close(void)
{
	socket_.close();
}

} // namespace mxasio

//int main(int argc, char* argv[])
//{
//	mxasio::SyncEchoClient::main(argc, argv);
//}
