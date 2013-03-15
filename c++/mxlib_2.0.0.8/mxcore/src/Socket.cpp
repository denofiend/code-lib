/*
 *        Name: Socket.cpp
 *
 *  Created on: 2011-2-11
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <string.h>
#include <iostream>
#include <mxcore/Assert.h>
#include <mxcore/Socket.h>
#include <mxcore/SocketOps.h>
#include <mxcore/IOUtil.h>
#include <mxcore/IOException.h>
#include <mxcore/MemRecord.h>

#if defined(WIN32)
#else
#include <errno.h>
#endif

namespace mxcore
{

Socket::Socket(void) :
	handle_(-1), state_(CLOSED), isBlocking_(true), local_(NULL), remote_(NULL)
{
}

Socket::~Socket(void)
{
	close();
}

void Socket::connect(const SocketAddress& remote, bool isBlocking)
		throw (IOException&)
{
	close();

	int sock = mxos::openSocket(AF_INET, SOCK_STREAM, 0);

	if (-1 == sock)
	{
		THROW3(IOException, "Can't open socket", mxos::getLastSocketError());
	}

	static SocketAddress localAny;

	if (0 != mxos::bind(sock, localAny.sockAddress(), localAny.length()))
	{
		mxos::closeSocket(sock);
		THROW3(IOException, "Can't bind socket", mxos::getLastSocketError());
	}

	if (!isBlocking)
	{
		try
		{
			IOUtil::configBlocking(sock, isBlocking);
			isBlocking_ = isBlocking;
		} catch (IOException& e)
		{
			mxos::closeSocket(sock);
			throw e;
		}
	}

	if (0 != mxos::connect(sock, remote.sockAddress(), remote.length()))
	{
		if (!isBlocking_)
		{
#if defined(WIN32)
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
			{
				state_ = CONNECTING;
				handle_ = sock;
				return;
			}
#else
			if (EINPROGRESS == errno)
			{
				state_ = CONNECTING;
				handle_ = sock;
				return;
			}
#endif
		}
		THROW3(mxcore::IOException, std::string("Can't connect to:[") + remote.toString() + "]", mxos::getLastSocketError());
	}
	else
	{
		state_ = CONNECTED;
	}

	handle_ = sock;
}

void Socket::shutdownInput(void) throw (IOException&)
{
	MX_ASSERT(!isClosed());

	mxos::shutdownInput(handle_);
}

void Socket::shutdownOutput(void) throw (IOException&)
{
	MX_ASSERT(!isClosed());

	mxos::shutdownOutput(handle_);
}

void Socket::close(void)
{
	if (-1 != handle_)
	{
		mxos::closeSocket(handle_);
		handle_ = -1;
		state_ = CLOSED;
		isBlocking_ = true;
	}

	if (NULL != local_)
	{
		delete local_;
		local_ = NULL;
	}

	if (NULL != remote_)
	{
		delete remote_;
		remote_ = NULL;
	}
}

bool Socket::isClosed(void) const
{
	return CLOSED == state_;
}

bool Socket::isConnected(void) const
{
	return CONNECTED == state_;
}

bool Socket::isConnecting(void) const
{
	return CONNECTING == state_;
}

void Socket::setRecvTimeout(int timeout)
{
	MX_ASSERT(!isClosed());
	::setsockopt(handle_, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout,
			sizeof(timeout));
}

void Socket::setSendTimeout(int timeout)
{
	MX_ASSERT(!isClosed());
	::setsockopt(handle_, SOL_SOCKET, SO_SNDTIMEO, (char*) &timeout,
			sizeof(timeout));
}

bool Socket::isBlocking(void) const
{
	return isBlocking_;
}

void Socket::configBlocking(bool blocking) throw (IOException&)
{
	MX_ASSERT(!isClosed());

	IOUtil::configBlocking(handle_, blocking);
	isBlocking_ = blocking;
}

void Socket::finishConnect(void) throw (IOException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(isConnecting());

	IOUtil::finishConnect(handle_, 0);
	state_ = CONNECTED;
}

int Socket::detach(void)
{
	int ret = handle_;
	handle_ = -1;
	state_ = CLOSED;
	isBlocking_ = true;

	if (NULL != local_)
	{
		delete local_;
		local_ = NULL;
	}

	if (NULL != remote_)
	{
		delete remote_;
		remote_ = NULL;
	}

	return ret;
}

void Socket::attach(int sock, bool isConnected, bool isBlocking)
{
	MX_ASSERT(-1 != sock);

	close();

	handle_ = sock;
	state_ = isConnected ? CONNECTED : CONNECTING;
	isBlocking_ = isBlocking;
}

int Socket::getHandle(void) const
{
	return handle_;
}

const SocketAddress& Socket::getLocalAddress(void) const throw (IOException&)
{
	if (isClosed())
	{
		MX_ASSERT(!isClosed());
	}
	if (NULL == local_)
	{
		struct sockaddr_in6 sa;
		size_t saLen = sizeof(struct sockaddr_in6);
		::memset(&sa, 0, sizeof(sa));

		if (0 != mxos::getsockname(handle_, (struct sockaddr*) &sa, &saLen))
		{
			THROW3(IOException, "Can't get sock name", mxos::getLastSocketError());
		}

		local_ = new SocketAddress;
		local_->set((struct sockaddr*) &sa, saLen);
	}

	return *local_;
}

const SocketAddress& Socket::getRemoteAddress(void) const throw (IOException&)
{
	MX_ASSERT(!isClosed());

	if (NULL == remote_)
	{
		struct sockaddr_in6 sa;
		size_t saLen = sizeof(struct sockaddr_in6);
		::memset(&sa, 0, sizeof(sa));

		if (0 != mxos::getpeername(handle_, (struct sockaddr*) &sa, &saLen))
		{
			THROW3(IOException, "Can't get peer name", mxos::getLastSocketError());
		}

		remote_ = new SocketAddress;
		remote_->set((struct sockaddr*) &sa, saLen);
	}

	return *remote_;
}

int Socket::read(void* buffer, int len) throw (IOException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != buffer && len > 0);

	int r = mxos::socketRead(handle_, buffer, len);

	if (r == 0)
	{
		return -1;
	}

	if (r < 0)
	{
#if defined(WIN32)
		if (::WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return 0;
		}

		THROW3(IOException, "Read error", ::WSAGetLastError());
#else
		if (EINPROGRESS == errno)
		{
			return 0;
		}

		THROW3(IOException, "Read error", errno);
#endif
	}
	return r;
}

int Socket::readN(void* dst, int n) throw (IOException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != dst && n > 0);

	int totalR = 0;
	char* p = (char*) dst;

	while (totalR < n)
	{
		int r = read(p, n - totalR);

		if (-1 == r)
		{
			break;
		}

		p += r;
		totalR += r;
	}

	return totalR;
}

int Socket::readV(mxos::IOVEC* vec, int count) throw (IOException&)
{
	MX_ASSERT(!isClosed());

	MX_ASSERT(NULL != vec && count > 0);

	int r = mxos::socketReadV(handle_, vec, count);

	if (r == 0)
	{
		return -1;
	}

	if (r < 0)
	{
#if defined(WIN32)
		if (::WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return 0;
		}

		THROW3(IOException, "Read error", ::WSAGetLastError());
#else
		if (EINPROGRESS == errno)
		{
			return 0;
		}

		THROW3(IOException, "Read error", errno);
#endif
	}
	return r;
}

int Socket::write(const void* buffer, int len) throw (IOException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != buffer && len > 0);

	int w = mxos::socketWrite(handle_, buffer, len);

	if (w <= 0)
	{
#if defined(WIN32)
		if (::WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return 0;
		}

		THROW3(IOException, "Write error", ::WSAGetLastError());
#else
		if (EINPROGRESS == errno)
		{
			return 0;
		}

		THROW3(IOException, "Write error", errno);
#endif
	}
	return w;
}

int Socket::writeN(const void* src, int n) throw (IOException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != src && n > 0);

	int totalW = 0;
	const char* p = (const char*) src;

	while (totalW < n)
	{
		int w = write(p, n - totalW);
		p += w;
		totalW += w;
	}
	return totalW;
}

int Socket::writeV(const mxos::IOVEC* vec, int count) throw (IOException&)
{
	MX_ASSERT(!isClosed());
	MX_ASSERT(NULL != vec && count > 0);

	int w = mxos::socketWriteV(handle_, vec, count);

	if (w <= 0)
	{
#if defined(WIN32)
		if (::WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return 0;
		}

		THROW3(IOException, "Write error", ::WSAGetLastError());
#else
		if (EINPROGRESS == errno)
		{
			return 0;
		}

		THROW3(IOException, "Write error", errno);
#endif
	}
	return w;
}

} // namespace mxcore
