/*
 *        Name: ChannelId.h
 *
 *  Created on: 2011-2-18
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXASIO_CHANNELID_H__
#define __MXASIO_CHANNELID_H__

#include <string>
#include <stdint.h>
#include <mxcore/Number.h>

namespace mxasio
{

class ChannelId
{
public:
	ChannelId(uint64_t sequence = 0);
	ChannelId(uint64_t sequence, int sock, const std::string& local,
			const std::string& remote);
	ChannelId(const ChannelId& src);
	ChannelId& operator =(const ChannelId& other);
	~ChannelId(void);

	uint64_t getSequence(void) const;
	int getSock(void) const;
	const std::string& getLocal(void) const;
	const std::string& getRemote(void) const;

	std::string toString(void) const;
private:
	uint64_t sequence_;
	int sock_;
	std::string local_;
	std::string remote_;
	std::string string_;
};

struct ChannelIdHash
{
	unsigned int operator()(const ChannelId& id) const
	{
		return (unsigned int) id.getSequence();
	}
};

struct ChannelIdEqual
{
	bool operator()(const ChannelId& x, const ChannelId& y) const
	{
		return x.getSequence() == y.getSequence() && x.getSock() == y.getSock()
				&& x.getLocal() == y.getLocal() && x.getRemote()
				== y.getRemote();
	}
};

inline uint64_t ChannelId::getSequence(void) const
{
	return sequence_;
}

inline int ChannelId::getSock(void) const
{
	return sock_;
}

inline const std::string& ChannelId::getLocal(void) const
{
	return local_;
}

inline const std::string& ChannelId::getRemote(void) const
{
	return remote_;
}

inline std::string ChannelId::toString(void) const
{
	return string_;
}

} // namespace mxasio

#endif /* __MXASIO_CHANNELID_H__ */
