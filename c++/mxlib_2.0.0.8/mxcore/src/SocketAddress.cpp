/*
 *        Name: SocketAddress.cpp
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <string.h>
#include <iostream>
#include <mxcore/Assert.h>
#include <mxcore/Number.h>
#include <mxcore/SocketAddress.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

SocketAddress::SocketAddress(void)
{
	::memset(&data_, 0, sizeof(data_));
	data_.base_.sa_family = AF_INET;
}

SocketAddress::SocketAddress(const SocketAddress& src)
{
	::memcpy(&data_, &src.data_, sizeof(data_));
}

SocketAddress::~SocketAddress(void)
{
}

SocketAddress& SocketAddress::operator =(const SocketAddress& other)
{
	if (this != &other)
	{
		::memcpy(&data_, &other.data_, sizeof(data_));
	}
	return *this;
}

bool SocketAddress::isV4(void) const
{
	return AF_INET == data_.base_.sa_family;
}

bool SocketAddress::isV6(void) const
{
	return AF_INET6 == data_.base_.sa_family;
}

void SocketAddress::setPort(uint16_t port)
{
	if (isV4())
	{
		data_.in4_.sin_port = htons(port);
	}
	else
	{
		data_.in6_.sin6_port = htons(port);
	}
}

uint16_t SocketAddress::getPort(void) const
{
	return isV4() ? ntohs(data_.in4_.sin_port) : ntohs(data_.in6_.sin6_port);
}

const struct sockaddr* SocketAddress::sockAddress(void) const
{
	return &data_.base_;
}

size_t SocketAddress::length(void) const
{
	return AF_INET == data_.base_.sa_family ? sizeof(struct sockaddr_in)
			: sizeof(struct sockaddr_in6);
}

const void* SocketAddress::address(void) const
{
	return isV6() ? (const void*) &data_.in6_.sin6_addr
			: (const void*) &data_.in4_.sin_addr;
}

size_t SocketAddress::addressLength(void) const
{
	return isV6() ? sizeof(struct in6_addr) : sizeof(struct in_addr);
}

void SocketAddress::address(const void* address, size_t len)
{
	if (sizeof(struct in_addr) == len)
	{
		::memcpy(&data_.in4_.sin_addr, address, len);
	}
	else if (sizeof(struct in6_addr) == len)
	{
		::memcpy(&data_.in6_.sin6_addr, address, len);
	}
	else
	{
		MX_ASSERT(0 && "Invalid addr len");
	}
}

void SocketAddress::set(const struct sockaddr* sa, size_t saLength)
{
	if (saLength == sizeof(struct sockaddr_in6))
	{
		::memcpy(&data_.in6_, sa, saLength);
		data_.base_.sa_family = AF_INET6;
	}
	else
	{
		::memcpy(&data_.in4_, sa, saLength);
		data_.base_.sa_family = AF_INET;
	}
}

void SocketAddress::set(const void* addr, size_t len, uint16_t port)
{
	setPort(port);
	address(addr, len);
}

static bool getAddrInfo(const char* hostname, int af, void* addr, int* addr_len)
{
	bool ret = false;
	struct addrinfo hints;
	struct addrinfo* res = NULL;

	MX_ASSERT(NULL != hostname);

	::memset(&hints, 0, sizeof(hints));
	hints.ai_family = af;

	if (0 == ::getaddrinfo(hostname, NULL, &hints, &res) && (res->ai_addrlen
			<= sizeof(struct sockaddr_in6)))
	{
		ret = true;
		*addr_len = res->ai_addrlen;
		::memcpy(addr, res->ai_addr, res->ai_addrlen);
	}

	if (NULL != res)
	{
		::freeaddrinfo(res);
	}

	return ret;
}

void SocketAddress::set(const char* hostname, int family, uint16_t port)
		throw (UnknownHostException)
{
	int addrLen = 0;

	MX_ASSERT(NULL != hostname);

	if (AF_UNSPEC == family)
	{
		if (!getAddrInfo(hostname, AF_INET6, &data_, &addrLen) && !getAddrInfo(
				hostname, AF_INET, &data_, &addrLen))
		{
			THROW2(UnknownHostException, std::string("Unknown hostname: [") + hostname + "]");
		}
	}
	else
	{
		if (!getAddrInfo(hostname, family, &data_, &addrLen))
		{
			THROW2(UnknownHostException, std::string("Unknown hostname: [") + hostname + "]");
		}
	}

	setPort(port);
}

void SocketAddress::set(const std::string& hostname, int family, uint16_t port)
		throw (UnknownHostException)
{
	MX_ASSERT(!hostname.empty());
	set(hostname.c_str(), family, port);
}

std::string SocketAddress::toString(void) const
{
	char buffer[mxos::MAX_ADDR_V6_LEN + 1] =
	{ 0 };

	UShort port = getPort();

	if (isV6())
	{
		if (NULL != mxos::inetNtoP(AF_INET6, (void*) &data_.in6_.sin6_addr,
				buffer, mxos::MAX_ADDR_V6_LEN, 0))
		{
			std::string ret("[");
			ret += buffer;
			ret += "]:";
			ret += port.toString();
			return buffer;
		}
	}
	else
	{
		if (NULL != mxos::inetNtoP(AF_INET, (void*) &data_.in4_.sin_addr,
				buffer, mxos::MAX_ADDR_V6_LEN, 0))
		{

			std::string ret(buffer);
			ret += ":";
			ret += port.toString();
			return ret;
		}

	}
	return "";
}

std::string SocketAddress::toAddressString(void) const
{
	char buffer[mxos::MAX_ADDR_V6_LEN + 1] =
	{ 0 };

	if (isV6())
	{
		if (NULL != mxos::inetNtoP(AF_INET6, (void*) &data_.in6_.sin6_addr,
				buffer, mxos::MAX_ADDR_V6_LEN, 0))
		{
			return buffer;
		}
	}
	else
	{
		if (NULL != mxos::inetNtoP(AF_INET, (void*) &data_.in4_.sin_addr,
				buffer, mxos::MAX_ADDR_V6_LEN, 0))
		{
			return buffer;
		}
	}
	return "";
}

bool SocketAddress::isAny(void) const
{
	if (AF_INET6 == data_.base_.sa_family)
	{
		return IN6_IS_ADDR_UNSPECIFIED(&data_.in6_.sin6_addr);
	}
	return (INADDR_ANY == data_.in4_.sin_addr.s_addr);
}

bool SocketAddress::isLoopback(void) const
{
	if (AF_INET6 == data_.base_.sa_family)
	{
		return IN6_IS_ADDR_LOOPBACK(&data_.in6_.sin6_addr);
	}
	return ((ntohl(data_.in4_.sin_addr.s_addr) & 0XFF000000)
			== (INADDR_LOOPBACK & 0XFF000000));
}

bool SocketAddress::isMulticast(void) const
{
	if (AF_INET6 == data_.base_.sa_family)
	{
		return data_.in6_.sin6_addr.s6_addr[0] == 0xFF;
	}

	return ntohl(data_.in4_.sin_addr.s_addr) >= 0XE000000 && ntohl(
			data_.in4_.sin_addr.s_addr) <= 0XEFFFFFFF;
}

bool SocketAddress::isLinkLocal(void) const
{
	if (AF_INET6 == data_.base_.sa_family)
	{
		return IN6_IS_ADDR_LINKLOCAL(&data_.in6_.sin6_addr);
	}
	return false;
}

bool SocketAddress::isMappedIpv6(void) const
{
	if (AF_INET6 == data_.base_.sa_family)
	{
		return IN6_IS_ADDR_V4MAPPED(&data_.in6_.sin6_addr);
	}
	return false;
}

bool SocketAddress::isCompatIpv6(void) const
{
	if (AF_INET6 == data_.base_.sa_family)
	{
		return IN6_IS_ADDR_V4COMPAT(&data_.in6_.sin6_addr);
	}
	return false;
}

SocketAddress SocketAddress::loopback(int family, uint16_t port)
{
	SocketAddress ret;
	if (AF_INET6 == family)
	{
		static const struct in6_addr sLoopbackV6 = IN6ADDR_LOOPBACK_INIT;
		ret.set(&sLoopbackV6, sizeof(struct in6_addr), port);
	}
	else
	{
		static uint32_t sLoopbackV4 = htonl(INADDR_LOOPBACK);
		ret.set(&sLoopbackV4, sizeof(sLoopbackV4), port);
	}

	return ret;
}

} // namespace mxcore
