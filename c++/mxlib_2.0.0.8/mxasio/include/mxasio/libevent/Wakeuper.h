/*
 *        Name: Wakeuper.h
 *
 *  Created on: 2011-2-21
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXASIO_LIBEVENT_WAKEUPER_H__
#define __MXASIO_LIBEVENT_WAKEUPER_H__

#include <event.h>
#include <mxcore/Atom.h>
#include <mxcore/IOException.h>

namespace mxasio
{

class Wakeuper
{
public:
	Wakeuper(void);
	~Wakeuper(void);

	void close(void);
	void open(struct event_base* evbase) throw (mxcore::IOException&);

	void wakeup(void);
private:
	static void onRead(int fd, short event, void *arg);
private:
	int pipe_[2];
	struct event readEvt_;
	struct event_base* evbase_;

	mxcore::AtomBool triggleWakeup_;
};

} // namespace mxasio

#endif /* __MXASIO_LIBEVENT_WAKEUPER_H__ */
