/*
 *        Name: IOUtil.cpp
 *
 *  Created on: 2011-2-11
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#if defined(WIN32)
#include <winsock2.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include <iostream>
#include <mxcore/SocketOps.h>

#include <mxcore/Number.h>
#include <mxcore/IOUtil.h>
#include <mxcore/Socket.h>
#include <mxcore/ServerSocket.h>
#include <mxcore/SocketAddress.h>

#include <mxcore/MemRecord.h>

namespace mxcore
{

void IOUtil::configBlocking(int sock, bool blocking) throw (IOException&)
{
	MX_ASSERT(-1 != sock);
#if defined(WIN32)
	unsigned long flag = blocking ? 1 : 0;

	if (0 != ::ioctlsocket(sock, FIONBIO, &flag))
	{
		if (blocking)
		{
			THROW3(IOException, std::string("Can't config blocking for fd: ") + Integer(sock).toString(), mxos::getLastSocketError());
		}
		else
		{
			THROW3(IOException, std::string("Can't config nonblocking for fd: ") + Integer(sock).toString(), mxos::getLastSocketError());
		}
	}
#else
	int flag = ::fcntl(sock, F_GETFL, 0);

	if ((O_NONBLOCK & flag) && blocking)
	{
		if (-1 == ::fcntl(sock, F_SETFL, flag & ~O_NONBLOCK))
		{
			THROW3(IOException, std::string("Can't config blocking for fd: ") + Integer(sock).toString(), mxos::getLastSocketError());
		}
	}
	else if (!(O_NONBLOCK & flag) && !blocking)
	{
		if (-1 == ::fcntl(sock, F_SETFL, flag | O_NONBLOCK))
		{
			THROW3(IOException, std::string("Can't config nonblocking for fd: ") + Integer(sock).toString(), mxos::getLastSocketError());
		}
	}
#endif
}

void IOUtil::createPipe(int pipefds[2]) throw (IOException&)
{
#if defined(WIN32)
	createSocketPair(pipefds);
	if (0 != mxos::shutdownOutput(pipefds[0]) || 0 != mxos::shutdownInput(
			pipefds[1]))
	{
		mxos::closeSocket(pipefds[0]);
		mxos::closeSocket(pipefds[1]);
		THROW3(IOException, std::string("Can't create pipe"), mxos::getLastSocketError());
	}
#else
	if (0 != ::pipe(pipefds))
	{
		THROW3(IOException, std::string("Can't create pipe"), mxos::getLastSocketError());
	}
#endif
}

void IOUtil::wakeup(int sock)
{
	char c = 1;

	MX_ASSERT(-1 != sock);

	for (;;)
	{
		int w = mxos::socketWrite(sock, &c, 1);

		if (w > 0)
		{
			break;
		}

		int err = mxos::getLastSocketError();
#if defined(WIN32)
		if (WSAEWOULDBLOCK == err)
		{
			break;
		}

		if (WSAEINTR == err)
		{
			continue;
		}
#else
		if (EAGAIN == err)
		{
			break;
		}

		if (EINTR == err)
		{
			continue;
		}
#endif
		break;
	}
}

void IOUtil::drain(int sock)
{
	int n = 128;
	char buf[128];

	MX_ASSERT(-1 != sock);

	for (; n == 128;)
	{
		n = mxos::socketRead(sock, buf, 128);

		if (n < 0)
		{
			int err = mxos::getLastSocketError();
#if defined(WIN32)
			if (WSAEWOULDBLOCK == err)
			{
				break;
			}

			if (WSAEINTR == err)
			{
				continue;
			}
#else
			if (EAGAIN == err)
			{
				break;
			}

			if (EINTR == err)
			{
				continue;
			}
#endif
		}
	}
}

void IOUtil::createSocketPair(int pair[2]) throw (IOException&)
{
#if defined(WIN32)
	Socket connectSocket;
	Socket serverClientSocket;
	ServerSocket listenSocket;

	SocketAddress loopback = SocketAddress::loopback(AF_INET, 0);
	listenSocket.listen(loopback, 5, false);
	connectSocket.connect(listenSocket.getLocalAddress(), true);

	if (!listenSocket.accept(serverClientSocket))
	{
		THROW2(IOException, "Can't accept for socket pair");
	}

	pair[0] = connectSocket.detach();
	pair[1] = serverClientSocket.detach();
#else
	if (0 != ::socketpair(AF_LOCAL, SOCK_STREAM, 0, pair))
	{
		THROW2(IOException, "Can't create socket pair");
	}
#endif
}

void IOUtil::finishConnect(int sock, int timeoutMs) throw (IOException&)
{
	int err;
	fd_set w;
	struct timeval tv;

	MX_ASSERT(-1 != sock);

	FD_ZERO(&w);
	FD_SET(sock, &w);

	tv.tv_sec = timeoutMs / 1000;
	tv.tv_usec = (timeoutMs % 1000) * 1000;

	err = ::select(sock + 1, NULL, &w, NULL, &tv);

	if (err <= 0 || !FD_ISSET(sock, &w))
	{
		THROW2(IOException, "Can't finish connect");
	}
}

} // namespace mxcore
