/*
 *        Name: SocketAddress.h
 *
 *  Created on: 2011-1-31
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_SOCKETADDRESS_H__
#define __MXCORE_SOCKETADDRESS_H__

#include <stdint.h>
#include <mxcore/SocketOps.h>
#include <mxcore/UnknownHostException.h>

namespace mxcore
{

class SocketAddress
{
public:
	SocketAddress(void);
	SocketAddress(const SocketAddress& src);
	~SocketAddress(void);

	SocketAddress& operator =(const SocketAddress& other);

	bool isV4(void) const;
	bool isV6(void) const;

	void setPort(uint16_t port);
	uint16_t getPort(void) const;

	const struct sockaddr* sockAddress(void) const;
	size_t length(void) const;

	const void* address(void) const;
	size_t addressLength(void) const;
	void address(const void* address, size_t len);

	void set(const struct sockaddr* sa, size_t saLength);
	void set(const void* address, size_t len, uint16_t port);

	void set(const char* hostname, int family, uint16_t port)
			throw (UnknownHostException);
	void set(const std::string& hostname, int family, uint16_t port)
			throw (UnknownHostException);

	std::string toString(void) const;
	std::string toAddressString(void) const;

	bool isAny(void) const;
	bool isLoopback(void) const;
	bool isMulticast(void) const;
	bool isLinkLocal(void) const;
	bool isMappedIpv6(void) const;
	bool isCompatIpv6(void) const;

	static SocketAddress loopback(int family, uint16_t port);
private:
	union
	{
		struct sockaddr base_;
		struct sockaddr_in in4_;
		struct sockaddr_in6 in6_;
	} data_;
};

} // namespace mxcore

#endif /* __MXCORE_SOCKETADDRESS_H__ */
