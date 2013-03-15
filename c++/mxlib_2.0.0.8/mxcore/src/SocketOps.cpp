/*
 *        Name: SocketOps.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/Assert.h>
#include <mxcore/SocketOps.h>
#include <mxcore/MemRecord.h>

namespace mxos
{

#if defined(WIN32)
namespace internal
{
class _NetInit
{
public:
	_NetInit(void)
	{
		WSADATA wsa_data;

		if (::WSAStartup(MAKEWORD(2,2), &wsa_data) != 0)
		{
			MX_ASSERT(0);
		}
	}

	~_NetInit(void)
	{
		::WSACleanup();
	}
};

static _NetInit sNetInit;

}
#endif

char* inetNtoP(int af, const void* src, char* dst, size_t length,
		unsigned long scopeId)
{
	MX_ASSERT(AF_INET == af || AF_INET6 == af);
	MX_ASSERT(NULL != src);
	MX_ASSERT(NULL != dst);
#if defined(WIN32)
	union
	{
		struct sockaddr base;
		struct sockaddr_in v4;
		struct sockaddr_in6 v6;
	} address;

	DWORD address_length = 0;

	if (af == AF_INET)
	{
		address_length = sizeof(sockaddr_in);
		address.v4.sin_family = AF_INET;
		address.v4.sin_port = 0;
		memcpy(&address.v4.sin_addr, src, sizeof(sockaddr_in));
	}
	else // AF_INET6
	{
		address_length = sizeof(sockaddr_in6);
		address.v6.sin6_family = AF_INET6;
		address.v6.sin6_port = 0;
		address.v6.sin6_flowinfo = 0;
		address.v6.sin6_scope_id = scopeId;
		memcpy(&address.v6.sin6_addr, src, sizeof(sockaddr_in6));
	}

	DWORD string_length = length;

	if (SOCKET_ERROR == ::WSAAddressToStringA(&address.base, address_length, 0,
			dst, &string_length))
	{
		return NULL;
	}

	dst[string_length] = '\0';
	return dst;
#else
	return (char*)::inet_ntop(af, src, dst, length);
#endif
}

int inetPtoN(int af, const char* src, void* dst, unsigned long* scopeId)
{
	MX_ASSERT(AF_INET == af || AF_INET6 == af);
	MX_ASSERT(NULL != src);
	MX_ASSERT(NULL != dst);

#if defined(WIN32)
	union
	{
		struct sockaddr base;
		struct sockaddr_in v4;
		struct sockaddr_in6 v6;
	} address;

	int address_length = sizeof(struct sockaddr_in6);
	::memset(&address, 0, sizeof(struct sockaddr_in6));

	int result = ::WSAStringToAddressA(const_cast<char*> (src), af, 0,
			&address.base, &address_length);

	if (af == AF_INET)
	{
		if (result != SOCKET_ERROR)
		{
			::memcpy(dst, &address.v4.sin_addr, sizeof(struct in_addr));
		}
		else if (strcmp(src, "255.255.255.255") == 0)
		{
			static_cast<struct in_addr*> (dst)->s_addr = INADDR_NONE;
		}
	}
	else // AF_INET6
	{
		if (result != SOCKET_ERROR)
		{
			::memcpy(dst, &address.v6.sin6_addr, sizeof(struct in6_addr));

			if (NULL != scopeId)
			{
				*scopeId = address.v6.sin6_scope_id;
			}
		}
	}

	return result == SOCKET_ERROR ? -1 : 0;
#else //
	if (::inet_pton(af, src, dst) <= 0)
	{
		return -1;
	}

	return 0;
#endif //
}

} // namespace mxos
