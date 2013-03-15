/*
 *        Name: SocketOps.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXOS_SOCKETOPS_H__
#define __MXOS_SOCKETOPS_H__

#include <mxcore/Assert.h>

#if defined(WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>

#else
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <sys/uio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#endif

namespace mxos
{

#if defined(WIN32)
typedef struct IOVEC
{
	unsigned long iov_len;
	void* iov_base;
} IOVEC;
#else
typedef struct IOVEC
{
	void* iov_base;
	size_t iov_len;
}IOVEC;
#endif
static const int MAX_ADDR_V6_LEN = 255;

char* inetNtoP(int af, const void* src, char* dst, size_t length,
		unsigned long scopeId);

int inetPtoN(int af, const char* src, void* dest, unsigned long* scopeId);

inline int getLastSocketError(void)
{
#if defined(WIN32)
	return ::WSAGetLastError();
#else
	return errno;
#endif
}

inline int openSocket(int domain, int type, int protocol)
{
#if defined(WIN32)
	return ::WSASocket(domain, type, protocol, 0, 0, 0);
#else
	return ::socket(domain, type, protocol);
#endif
}

inline int bind(int sock, const struct sockaddr* addr, size_t addrLen)
{
	MX_ASSERT(-1 != sock);
	return ::bind(sock, addr, addrLen);
}

inline int listen(int sock, int backlog)
{
	MX_ASSERT(-1 != sock);
	return ::listen(sock, backlog);
}

inline int accept(int sock, struct sockaddr* addr, size_t* addrLen)
{
	MX_ASSERT(-1 != sock);

	if (NULL != addr)
	{
		MX_ASSERT(NULL != addrLen);
	}
#if defined(WIN32)
	if (NULL == addr)
	{
		return ::WSAAccept(sock, NULL, 0, NULL, 0);
	}
	else
	{
		int len = *addrLen;
		int client = ::WSAAccept(sock, addr, &len, NULL, 0);
		*addrLen = len;
		return client;
	}
#else
	if (NULL == addr)
	{
		return ::accept(sock, NULL, 0);
	}
	else
	{
		socklen_t len = *addrLen;
		int client = ::accept(sock, addr, &len);
		*addrLen = len;
		return client;
	}
#endif
}

inline int connect(int sock, const struct sockaddr* addr, size_t addrLen)
{
	MX_ASSERT(-1 != sock);
	MX_ASSERT(NULL != addr);
#if defined(WIN32)
	return ::WSAConnect(sock, addr, addrLen, NULL, NULL, NULL, NULL);
#else
	return ::connect(sock, addr, addrLen);
#endif
}

inline int shutdownInput(int sock)
{
	MX_ASSERT(-1 != sock);
#if defined(WIN32)
	return ::shutdown(sock, SD_RECEIVE);
#else
	return ::shutdown(sock, SHUT_RD);
#endif
}

inline int shutdownOutput(int sock)
{
	MX_ASSERT(-1 != sock);
#if defined(WIN32)
	return ::shutdown(sock, SD_SEND);
#else
	return ::shutdown(sock, SHUT_WR);
#endif
}

inline void closeSocket(int sock)
{
	if (-1 != sock)
	{
#if defined(WIN32)
		::closesocket(sock);
#else
		::close(sock);
#endif
	}
}

inline int socketRead(int sock, void* dst, int length)
{
	MX_ASSERT(-1 != sock);
	MX_ASSERT(NULL != dst && length > 0);
#if defined(WIN32)
	return ::recv(sock, (char*) dst, length, 0);
#else
	return ::read(sock, dst, length);
#endif
}

inline int socketReadV(int sock, struct IOVEC* vec, int count)
{
	MX_ASSERT(-1 != sock);
	MX_ASSERT(NULL != vec);
	MX_ASSERT(count > 0);
#if defined(WIN32)
	return ::WSARecv(sock, (WSABUF*) vec, count, NULL, 0, 0, 0);
#else
	return ::readv(sock, (struct iovec*)vec, count);
#endif
}

inline int socketReadN(int sock, void* dst, int n)
{
	MX_ASSERT(-1 != sock);
	MX_ASSERT(NULL != dst && n > 0);

	int remaining = n;
	char* p = (char*) dst;

	do
	{
		int r = socketRead(sock, p, remaining);

		if (r <= 0)
		{
			break;
		}

		p += r;
		remaining -= r;
	} while (remaining > 0);

	return n - remaining;
}

inline int socketWrite(int sock, const void* src, int length)
{
	MX_ASSERT(-1 != sock);
	MX_ASSERT(NULL != src && length > 0);
#if defined(WIN32)
	return ::send(sock, (const char*) src, length, 0);
#else
	return ::write(sock, src, length);
#endif
}

inline int socketWriteV(int sock, const struct IOVEC* vec, int count)
{
	MX_ASSERT(-1 != sock);
	MX_ASSERT(NULL != vec);
	MX_ASSERT(count > 0);
#if defined(WIN32)
	return ::WSASend(sock, (WSABUF*) vec, count, NULL, 0, 0, 0);
#else
	return ::writev(sock, (const struct iovec*)vec, count);
#endif
}

inline int socketWriteN(int sock, const void* src, int n)
{
	MX_ASSERT(-1 != sock);
	MX_ASSERT(NULL != src && n > 0);

	const char* p = (const char*) src;
	int remaining = n;

	do
	{
		int w = socketWrite(sock, p, remaining);

		if (w <= 0)
		{
			break;
		}

		p += w;
		remaining -= w;
	} while (remaining > 0);

	return n - remaining;
}

inline int getsockname(int sock, struct sockaddr* sa, size_t* saLen)
{
	MX_ASSERT(-1 != sock);
#if defined(WIN32)
	int len = *saLen;
	int ret = ::getsockname(sock, sa, &len);
	*saLen = len;
	return ret;
#else
	socklen_t len = *saLen;
	int ret = ::getsockname(sock, sa, &len);
	*saLen = len;
	return ret;
#endif
}

inline int getpeername(int sock, struct sockaddr* sa, size_t* saLen)
{
	MX_ASSERT(-1 != sock);
#if defined(WIN32)
	int len = *saLen;
	int ret = ::getpeername(sock, sa, &len);
	*saLen = len;
	return ret;
#else
	socklen_t len = *saLen;
	int ret = ::getpeername(sock, sa, &len);
	*saLen = len;
	return ret;
#endif
}

inline int setsockopt(int sock, int level, int optname, const void* optVal,
		size_t optLen)
{
	MX_ASSERT(-1 != sock);
#if defined(WIN32)
	return ::setsockopt(sock, level, optname, (const char*) optVal, optLen);
#else
	return ::setsockopt(sock, level, optname, (const char*) optVal, optLen);
#endif
}

} // namespace mxcore

#endif /* __MXOS_SOCKETOPS_H__ */
