/*
 *        Name: ChannelId.cpp
 *
 *  Created on: 2011-2-18
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxasio/ChannelId.h>
#include <mxcore/MemRecord.h>

namespace mxasio
{

ChannelId::ChannelId(uint64_t sequence) :
	sequence_(sequence), sock_(-1)
{
}

ChannelId::ChannelId(uint64_t sequence, int sock, const std::string& local,
		const std::string& remote) :
	sequence_(sequence), sock_(sock), local_(local), remote_(remote)
{
	if (remote.empty())
	{
		string_ = "A_";
	}
	else
	{
		string_ = "C_";
	}

	string_ += mxcore::Integer(sock_).toString();
	string_ += "_";
	string_ += mxcore::ULong(sequence_).toString();
	string_ += "_";

	if (!remote.empty())
	{
		string_ += remote;
	}
	else
	{
		string_ += local;
	}
}

ChannelId::ChannelId(const ChannelId& src) :
	sequence_(src.sequence_), sock_(src.sock_), local_(src.local_),
			remote_(src.remote_), string_(src.string_)
{
}

ChannelId& ChannelId::operator =(const ChannelId& other)
{
	if (this != &other)
	{
		sequence_ = other.sequence_;
		sock_ = other.sock_;
		local_ = other.local_;
		remote_ = other.remote_;
		string_ = other.string_;
	}
	return *this;
}

ChannelId::~ChannelId(void)
{
}

} // namespace mxasio
