/*
 *        Name: ChannelMessage.cpp
 *
 *  Created on: 2011-2-23
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxasio/ChannelMessage.h>
#include <mxcore/MemRecord.h>

namespace mxasio
{

ChannelMessage::ChannelMessage(void) :
	timeout_(0), isReadAll_(false)
{
}

ChannelMessage::ChannelMessage(const ChannelMessage& src) :
	timeout_(src.timeout_), isReadAll_(src.isReadAll_), buffer_(src.buffer_)
{
}

ChannelMessage::ChannelMessage(mxcore::SharedPtr<mxcore::ByteBuffer> buf,
		int timeout, bool isReadAll) :
	timeout_(timeout), isReadAll_(isReadAll), buffer_(buf)
{
}

ChannelMessage& ChannelMessage::operator =(const ChannelMessage& other)
{
	if (this != &other)
	{
		timeout_ = other.timeout_;
		isReadAll_ = other.isReadAll_;
		buffer_ = other.buffer_;
	}
	return *this;
}

ChannelMessage::~ChannelMessage(void)
{
}

int ChannelMessage::getTimeout(void) const
{
	return timeout_;
}

bool ChannelMessage::isAll(void) const
{
	return isReadAll_;
}

mxcore::SharedPtr<mxcore::ByteBuffer>& ChannelMessage::getBuffer(void)
{
	return buffer_;
}

const mxcore::SharedPtr<mxcore::ByteBuffer>& ChannelMessage::getBuffer(void) const
{
	return buffer_;
}

} // namespace mxasio
