/*
 * SyncEchoClient.h
 *
 *  Created on: 2011-3-4
 *      Author: zhaojiangang
 */

#ifndef SAMPLE_MXASIO_SYNCECHOCLIENT_H_
#define SAMPLE_MXASIO_SYNCECHOCLIENT_H_

#include <mxcore/Socket.h>

namespace mxasio
{

class SyncEchoClient
{
public:
	SyncEchoClient(void);
	virtual ~SyncEchoClient(void);

	static int main(int argc, char* argv[]);

	void connect(const mxcore::SocketAddress& remote)
			throw (mxcore::IOException&);
	bool sendAndRecv(const std::string& message) throw (mxcore::IOException&);
	void close(void);
private:
	mxcore::Socket socket_;
};

} // namespace mxasio

#endif /* SAMPLE_MXASIO_SYNCECHOCLIENT_H_ */
