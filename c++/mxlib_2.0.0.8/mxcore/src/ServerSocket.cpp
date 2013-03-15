/*
 *        Name: ServerSocket.cpp
 *
 *  Created on: 2011-2-11
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <string.h>
#include <mxcore/IOUtil.h>
#include <mxcore/ServerSocket.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

ServerSocket::ServerSocket(void) :
	handle_(-1), isBlocking_(true), local_(NULL)
{
}

ServerSocket::~ServerSocket(void)
{
	close();
}

void ServerSocket::listen(const SocketAddress& local, int backlog,
		bool reuseAddr, bool blocking) throw (IOException&)
{
	close();

	int sock = mxos::openSocket(AF_INET, SOCK_STREAM, 0);

	if (-1 == sock)
	{
		THROW3(IOException, "Can't open socket", mxos::getLastSocketError());
	}

	if (reuseAddr)
	{
		int val = 1;

		if (0 != mxos::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val,
				sizeof(val)))
		{
			mxos::closeSocket(sock);
			THROW3(IOException, "Can't reuse addr", mxos::getLastSocketError());
		}
	}

	if (0 != mxos::bind(sock, local.sockAddress(), local.length()))
	{
		mxos::closeSocket(sock);
		THROW3(IOException, std::string("Can't bind socket to:") + local.toString(), mxos::getLastSocketError());
	}

	if (!blocking)
	{
		try
		{
			IOUtil::configBlocking(sock, false);
		} catch (mxcore::IOException& e)
		{
			mxos::closeSocket(sock);
			throw e;
		}
	}
	if (0 != mxos::listen(sock, backlog))
	{
		mxos::closeSocket(sock);
		THROW3(IOException, "Can't listen socket", mxos::getLastSocketError());
	}

	handle_ = sock;
}

void ServerSocket::attach(int handle, bool isBlocking)
{
	MX_ASSERT(-1 != handle);

	close();

	handle_ = handle;
	isBlocking_ = isBlocking;
}

bool ServerSocket::accept(Socket& client) throw (IOException&)
{
	MX_ASSERT(!isClosed());

	int sock = mxos::accept(handle_, NULL, NULL);
	if (-1 == sock)
	{
		int err = mxos::getLastSocketError();
#if defined(WIN32)
		if (WSAEWOULDBLOCK == err || WSAEINTR == err)
		{
			return false;
		}
#else
		if (EAGAIN == err || EINTR == err)
		{
			return false;
		}
#endif
		THROW3 (IOException, "Can't accept socket", err);
	}

	client.attach(sock, true, true);
	return true;
}

void ServerSocket::close(void)
{
	if (-1 != handle_)
	{
		mxos::closeSocket(handle_);
		handle_ = -1;
		isBlocking_ = true;
	}

	if (NULL != local_)
	{
		delete local_;
		local_ = NULL;
	}
}

bool ServerSocket::isClosed(void) const
{
	return -1 == handle_;
}

bool ServerSocket::isBlocking(void) const
{
	return isBlocking_;
}

void ServerSocket::configBlocking(bool blocking) throw (IOException&)
{
	MX_ASSERT(!isClosed());

	IOUtil::configBlocking(handle_, blocking);
}

int ServerSocket::detach(void)
{
	int ret = handle_;
	handle_ = -1;
	isBlocking_ = true;

	if (NULL != local_)
	{
		delete local_;
		local_ = NULL;
	}

	return ret;
}

int ServerSocket::getHandle(void) const
{
	return handle_;
}

const SocketAddress& ServerSocket::getLocalAddress(void) const
		throw (IOException&)
{
	MX_ASSERT(!isClosed());

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

} // namespace mxcore
