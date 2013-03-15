/*
 *        Name: IOUtil.h
 *
 *  Created on: 2011-2-11
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXCORE_IOUTIL_H__
#define __MXCORE_IOUTIL_H__

#include <mxcore/IOException.h>
#include <mxcore/MessageBuffer.h>

namespace mxcore
{

class IOUtil
{
private:
	IOUtil(void);
public:
	static void configBlocking(int sock, bool blocking) throw (IOException&);
	static void createPipe(int pipe[2]) throw (IOException&);
	static void createSocketPair(int pair[2]) throw (IOException&);
	static void finishConnect(int sock, int timeoutMs) throw (IOException&);
	static void wakeup(int fd);
	static void drain(int fd);
};

} // namespace mxcore

#endif /* __MXCORE_IOUTIL_H__ */
