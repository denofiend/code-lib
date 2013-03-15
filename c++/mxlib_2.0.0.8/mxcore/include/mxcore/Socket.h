/*
 *        Name: Socket.h
 *
 *  Created on: 2011-2-11
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_SOCKET_H__
#define __MXCORE_SOCKET_H__

#include <mxcore/IOException.h>
#include <mxcore/SocketAddress.h>

namespace mxcore
{

class Socket
{
	friend class ServerSocket;
public:
	Socket(void);
	~Socket(void);

	void connect(const SocketAddress& remote, bool blocking = true)
			throw (IOException&);

	void shutdownInput(void) throw (IOException&);
	void shutdownOutput(void) throw (IOException&);
	void close(void);

	bool isClosed(void) const;
	bool isConnected(void) const;
	bool isConnecting(void) const;

	void setRecvTimeout(int timeout);
	void setSendTimeout(int timeout);

	bool isBlocking(void) const;
	void configBlocking(bool blocking) throw (IOException&);

	void finishConnect(void) throw (IOException&);

	int detach(void);
	void attach(int sock, bool isConnected, bool isBlocking);
	int getHandle(void) const;

	const SocketAddress& getLocalAddress(void) const throw (IOException&);
	const SocketAddress& getRemoteAddress(void) const throw (IOException&);

	/**
	 * @return
	 * 	-1 if eof, 0 if no data ready, otherwise the read bytes
	 */
	int read(void* dst, int len) throw (IOException&);
	int readN(void* dst, int n) throw (IOException&);
	int readV(mxos::IOVEC* vec, int count) throw (IOException&);

	int write(const void* src, int len) throw (IOException&);
	int writeN(const void* src, int n) throw (IOException&);
	int writeV(const mxos::IOVEC* vec, int count) throw (IOException&);
private:
	enum
	{
		CLOSED, CONNECTING, CONNECTED
	};

	int handle_;
	int state_;
	bool isBlocking_;

	mutable SocketAddress* local_;
	mutable SocketAddress* remote_;
};

} // namespace mxcore

#endif /* __MXCORE_SOCKET_H__ */
