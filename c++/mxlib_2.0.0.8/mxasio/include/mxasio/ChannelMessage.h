/*
 *        Name: ChannelMessage.h
 *
 *  Created on: 2011-2-23
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXASIO_CHANNELMESSAGE_H__
#define __MXASIO_CHANNELMESSAGE_H__

#include <mxcore/SharedPtr.h>
#include <mxcore/ByteBuffer.h>

namespace mxasio
{

class ChannelMessage
{
public:
	ChannelMessage(void);
	ChannelMessage(const ChannelMessage& src);
	ChannelMessage(mxcore::SharedPtr<mxcore::ByteBuffer> buf, int timeout,
			bool isReadAll = false);
	ChannelMessage& operator = (const ChannelMessage& other);
	virtual ~ChannelMessage(void);

	int getTimeout(void) const;
	bool isAll(void) const;
	mxcore::SharedPtr<mxcore::ByteBuffer>& getBuffer(void);
	const mxcore::SharedPtr<mxcore::ByteBuffer>& getBuffer(void) const;
private:
	int timeout_;
	bool isReadAll_;
	mxcore::SharedPtr<mxcore::ByteBuffer> buffer_;
};

} // namespace mxasio

#endif /* __MXASIO_CHANNELMESSAGE_H__ */
