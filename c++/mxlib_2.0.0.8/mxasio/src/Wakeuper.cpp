/*
 *        Name: Wakeuper.cpp
 *
 *  Created on: 2011-2-21
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <string.h>
#include <mxcore/IOUtil.h>
#include <mxcore/LoggerFactory.h>
#include <mxasio/libevent/Wakeuper.h>
#include <mxcore/MemRecord.h>

namespace mxasio
{

Wakeuper::Wakeuper(void) :
	triggleWakeup_(false)
{
	pipe_[0] = pipe_[1] = -1;
	evbase_ = NULL;
	::memset(&readEvt_, 0, sizeof(struct event));
}

Wakeuper::~Wakeuper(void)
{
	close();
}

void Wakeuper::close(void)
{
	if (NULL != evbase_)
	{
		triggleWakeup_.set(false);
		::event_del(&readEvt_);
		evbase_ = NULL;
		mxos::closeSocket(pipe_[0]);
		mxos::closeSocket(pipe_[1]);
		pipe_[0] = pipe_[1] = -1;
	}
}

void Wakeuper::open(struct event_base* evbase) throw (mxcore::IOException&)
{
	close();

	try
	{
		mxcore::IOUtil::createSocketPair(pipe_);
		mxcore::IOUtil::configBlocking(pipe_[0], false);
		mxcore::IOUtil::configBlocking(pipe_[1], false);

		::event_set(&readEvt_, pipe_[0], EV_READ | EV_PERSIST,
				Wakeuper::onRead, this);
		::event_base_set(evbase, &readEvt_);
		::event_add(&readEvt_, NULL);

		evbase_ = evbase;
	} catch (mxcore::IOException& e)
	{
		mxos::closeSocket(pipe_[0]);
		mxos::closeSocket(pipe_[1]);
		pipe_[0] = pipe_[1] = -1;

		throw e;
	}
}

void Wakeuper::wakeup(void)
{
	if (!triggleWakeup_.getAndSet(true))
	{
		mxcore::IOUtil::wakeup(pipe_[1]);
	}
}

void Wakeuper::onRead(int fd, short event, void *arg)
{
	Wakeuper* pThis = (Wakeuper*) arg;
	pThis->triggleWakeup_.set(false);
	mxcore::IOUtil::drain(pThis->pipe_[0]);
}

} // namespace mxasio
