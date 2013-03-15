/*
 *        Name: ServerSocket.h
 *
 *  Created on: 2011-2-11
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_SERVERSOCKET_H__
#define __MXCORE_SERVERSOCKET_H__

#include <mxcore/Socket.h>
#include <mxcore/SocketAddress.h>

namespace mxcore
{

class ServerSocket
{
public:
	ServerSocket(void);
	~ServerSocket(void);

	void listen(const SocketAddress& local, int backlog, bool reuseAddr = true,
			bool blocking = true) throw (IOException&);
	void attach(int handle, bool isBlocking = true);

	bool accept(Socket& client) throw (IOException&);
	void close(void);

	bool isClosed(void) const;

	bool isBlocking(void) const;
	void configBlocking(bool blocking) throw (IOException&);

	int detach(void);
	int getHandle(void) const;

	const SocketAddress& getLocalAddress(void) const throw (IOException&);
private:
	int handle_;
	bool isBlocking_;

	mutable SocketAddress* local_;
};

} // namespace mxcore

#endif /* __MXCORE_SERVERSOCKET_H__ */
