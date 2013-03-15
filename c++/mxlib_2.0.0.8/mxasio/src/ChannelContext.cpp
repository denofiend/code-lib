/*
 *        Name: ChannelContext.cpp
 *
 *  Created on: 2011-2-21
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <string.h>
#include <mxcore/Assert.h>
#include <mxcore/Number.h>

#include <mxasio/libevent/ChannelContext.h>
#include <mxasio/libevent/IOServiceLibEvent.h>
#include <mxcore/MemRecord.h>

namespace mxasio
{

ChannelContext::ChannelContext(int type, IOServiceLibEvent* service,
		const ChannelId& channelId, IOApplication* application,
		void* attachment) :
	type_(type), channelId_(channelId), service_(service),
			application_(application), attachment_(attachment)
{
}

ChannelContext::~ChannelContext(void)
{
}

bool ChannelContext::isAcceptor(void) const
{
	return eAcceptor == type_;
}

bool ChannelContext::isConnector(void) const
{
	return eConnector == type_;
}

AcceptorChannelContext::AcceptorChannelContext(IOServiceLibEvent* service,
		const ChannelId& channelId, IOApplication* application,
		void* attachment) :
			ChannelContext(ChannelContext::eAcceptor, service, channelId,
					application, attachment)
{
	::memset(&acceptEvt_, 0, sizeof(struct event));
}

AcceptorChannelContext::~AcceptorChannelContext(void)
{
}

void AcceptorChannelContext::registAccept(void)
{
	if (NULL == acceptEvt_.ev_base)
	{
		::event_set(&acceptEvt_, acceptor_.getHandle(), EV_READ | EV_PERSIST,
				IOServiceLibEvent::onAccept, this);
		::event_base_set(getService()->getEventBase(), &acceptEvt_);
		::event_add(&acceptEvt_, NULL);
	}
}

void AcceptorChannelContext::unregistAccept(void)
{
	if (NULL != acceptEvt_.ev_base)
	{
		::event_del(&acceptEvt_);
		::memset(&acceptEvt_, 0, sizeof(struct event));
	}
}

ConnectorChannelContext::ConnectorChannelContext(IOServiceLibEvent* service,
		const ChannelId& channelId, IOApplication* application,
		void* attachment) :
			ChannelContext(ChannelContext::eConnector, service, channelId,
					application, attachment)
{
	::memset(&readEvt_, 0, sizeof(struct event));
	::memset(&writeEvt_, 0, sizeof(struct event));
	::memset(&readTimer_, 0, sizeof(struct event));
	::memset(&writeTimer_, 0, sizeof(struct event));
}

ConnectorChannelContext::~ConnectorChannelContext(void)
{
}

mxcore::Socket& ConnectorChannelContext::getConnector(void)
{
	return connector_;
}

void ConnectorChannelContext::registRead(void)
{
	if (NULL == readEvt_.ev_base)
	{
		::event_set(&readEvt_, connector_.getHandle(), EV_READ | EV_PERSIST,
				IOServiceLibEvent::onRead, this);
		::event_base_set(getService()->getEventBase(), &readEvt_);
		::event_add(&readEvt_, NULL);
	}
}

void ConnectorChannelContext::registReadTimer(int timeout)
{
	unregistReadTimer();

	if (timeout > 0)
	{
		struct timeval tv;
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout - tv.tv_sec * 1000) * 1000;

		::evtimer_set(&readTimer_, IOServiceLibEvent::onReadTimeout,
				this);
		::event_base_set(getService()->getEventBase(), &readTimer_);
		::event_add(&readTimer_, &tv);
	}
}

void ConnectorChannelContext::registWrite(void)
{
	if (NULL == writeEvt_.ev_base)
	{
		::event_set(&writeEvt_, connector_.getHandle(), EV_WRITE | EV_PERSIST,
				IOServiceLibEvent::onWrite, this);
		::event_base_set(getService()->getEventBase(), &writeEvt_);
		::event_add(&writeEvt_, NULL);
	}
}

void ConnectorChannelContext::registWriteTimer(int timeout)
{
	unregistWriteTimer();

	if (timeout > 0)
	{
		struct timeval tv;
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout - tv.tv_sec * 1000) * 1000;

		::evtimer_set(&writeTimer_, IOServiceLibEvent::onWriteTimeout,
				this);
		::event_base_set(getService()->getEventBase(), &writeTimer_);
		::event_add(&writeTimer_, &tv);
	}
}

void ConnectorChannelContext::unregistRead(void)
{
	if (NULL != readEvt_.ev_base)
	{
		::event_del(&readEvt_);
		::memset(&readEvt_, 0, sizeof(struct event));
	}
}

void ConnectorChannelContext::unregistReadTimer(void)
{
	if (NULL != readTimer_.ev_base)
	{
		::event_del(&readTimer_);
		::memset(&readTimer_, 0, sizeof(struct event));
	}
}

void ConnectorChannelContext::unregistWrite(void)
{
	if (NULL != writeEvt_.ev_base)
	{
		::event_del(&writeEvt_);
		::memset(&writeEvt_, 0, sizeof(struct event));
	}
}

void ConnectorChannelContext::unregistWriteTimer(void)
{
	if (NULL != writeTimer_.ev_base)
	{
		::event_del(&writeTimer_);
		::memset(&writeTimer_, 0, sizeof(struct event));
	}
}

void ConnectorChannelContext::unregistAll(void)
{
	unregistRead();
	unregistReadTimer();
	unregistWrite();
	unregistWriteTimer();
}

static const int kMaxVec = 10;

int ConnectorChannelContext::readMessage(
		std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& readMsgList)
		throw (mxcore::IOException&)
{
	int ret = 0;
	mxos::IOVEC vec[kMaxVec];

	MX_ASSERT(!toReadMessageList_.empty());

	int n = (int) toReadMessageList_.size();

	n = n > kMaxVec ? kMaxVec : n;

	int i = 0;

	for (std::list<ChannelMessage>::const_iterator it =
			toReadMessageList_.begin(); i < n && it != toReadMessageList_.end(); i++, it++)
	{
		const ChannelMessage& msg = *it;
		vec[i].iov_base = msg.getBuffer()->getBuffer();
		vec[i].iov_len = msg.getBuffer()->remaining();
	}

	try
	{
		int r = connector_.readV(vec, n);

		if (-1 == r)
		{
			readMsgList.push_back(popFrontReadMessage().getBuffer());
			return -1;
		}

		ret = r;

		while (r > 0)
		{
			ChannelMessage& msg = toReadMessageList_.front();
			int
					subR =
							r > (int) msg.getBuffer()->remaining() ? (int) msg.getBuffer()->remaining()
									: r;

			msg.getBuffer()->setPosition(msg.getBuffer()->getPosition() + subR);
			r -= subR;

			if (msg.getBuffer()->remaining() == 0 || !msg.isAll())
			{
				msg.getBuffer()->flip();
				readMsgList.push_back(msg.getBuffer());
				toReadMessageList_.pop_front();
			}
		}

		return ret;
	} catch (mxcore::IOException& e)
	{
		readMsgList.push_back(popFrontReadMessage().getBuffer());
		throw e;
	}

}

void ConnectorChannelContext::writeMessage(
		std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >& writtenMsgList)
		throw (mxcore::IOException&)
{
	mxos::IOVEC vec[kMaxVec];

	MX_ASSERT(!toWriteMessageList_.empty());

	int n = (int) toWriteMessageList_.size();

	n = n > kMaxVec ? kMaxVec : n;

	int i = 0;

	for (std::list<ChannelMessage>::const_iterator it =
			toWriteMessageList_.begin(); i < n && it
			!= toWriteMessageList_.end(); it++, i++)
	{
		const ChannelMessage& msg = *it;
		vec[i].iov_base = msg.getBuffer()->getBuffer();
		vec[i].iov_len = msg.getBuffer()->remaining();
	}

	try
	{

		int w = connector_.writeV(vec, n);

		while (w > 0)
		{
			ChannelMessage& msg = toWriteMessageList_.front();
			int
					subW =
							w > (int) msg.getBuffer()->remaining() ? (int) msg.getBuffer()->remaining()
									: w;

			msg.getBuffer()->setPosition(msg.getBuffer()->getPosition() + subW);
			w -= subW;

			if (msg.getBuffer()->remaining() == 0 || !msg.isAll())
			{
				writtenMsgList.push_back(msg.getBuffer());
				toWriteMessageList_.pop_front();
			}
		}
	} catch (mxcore::IOException& e)
	{
		writtenMsgList.push_back(popFrontWriteMessage().getBuffer());
		throw e;
	}
}

bool ConnectorChannelContext::haveMessageToRead(void) const
{
	return !toReadMessageList_.empty();
}

bool ConnectorChannelContext::haveMessageToWrite(void) const
{
	return !toWriteMessageList_.empty();
}

ChannelMessage ConnectorChannelContext::frontReadMessage(void)
{
	return toReadMessageList_.front();
}

ChannelMessage ConnectorChannelContext::popFrontReadMessage(void)
{
	ChannelMessage msg = toReadMessageList_.front();
	toReadMessageList_.pop_front();
	return msg;
}

ChannelMessage ConnectorChannelContext::frontWriteMessage(void)
{
	return toWriteMessageList_.front();
}

ChannelMessage ConnectorChannelContext::popFrontWriteMessage(void)
{
	ChannelMessage msg = toWriteMessageList_.front();
	toWriteMessageList_.pop_front();
	return msg;
}

void ConnectorChannelContext::popAllReadMessage(void)
{
	toReadMessageList_.clear();
}

void ConnectorChannelContext::popAllWriteMessage(void)
{
	toWriteMessageList_.clear();
}

void ConnectorChannelContext::putReadMessage(const ChannelMessage& msg)
{
	toReadMessageList_.push_back(msg);
}

void ConnectorChannelContext::putWriteMessage(const ChannelMessage& msg)
{
	toWriteMessageList_.push_back(msg);
}

void ConnectorChannelContext::putWriteMessage(
		const std::list<mxcore::SharedPtr<mxcore::ByteBuffer> > messageList,
		int perMsgTimeout)
{
	for (std::list<mxcore::SharedPtr<mxcore::ByteBuffer> >::const_iterator it =
			messageList.begin(); it != messageList.end(); it++)
	{
		toWriteMessageList_.push_back(ChannelMessage(*it, perMsgTimeout, true));
	}
}

} // namespace mxasio
