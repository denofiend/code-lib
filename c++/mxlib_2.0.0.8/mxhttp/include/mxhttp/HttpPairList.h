/*
 *        Name: HttpPairList.h
 *
 *  Created on: 2011-2-25
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#ifndef __MXHTTP_HTTPPAIRLIST_H__
#define __MXHTTP_HTTPPAIRLIST_H__

#include <vector>
#include <mxcore/Assert.h>
#include <mxcore/StringUtil.h>

namespace mxhttp
{

typedef std::pair<std::string, std::string> HttpPair;

class HttpPairList
{
public:
	HttpPairList(void);
	~HttpPairList(void);

	int getCount(void) const;

	const HttpPair& get(int index) const;
	const std::string& getValue(const std::string& key) const;

	bool isExists(const std::string& key) const;
	int find(const std::string& key) const;

	void add(const HttpPair& kv);
	void add(const std::string& key, const std::string& value);

	bool remove(const std::string& key);
private:
	std::vector<HttpPair> pairs_;
};

typedef HttpPair HttpParam;
typedef HttpPairList HttpParamList;

typedef HttpPair HttpHeader;
typedef HttpPairList HttpHeaderList;

inline int HttpPairList::getCount(void) const
{
	return (int) pairs_.size();
}

inline const HttpPair& HttpPairList::get(int index) const
{
	MX_ASSERT(index >= 0 && index < getCount());
	return pairs_[index];
}

inline const std::string& HttpPairList::getValue(const std::string& key) const
{
	int index = find(key);
	if (-1 == index)
	{
		static std::string sEmpty("");
		return sEmpty;
	}
	return pairs_[index].second;
}

inline bool HttpPairList::isExists(const std::string& key) const
{
	return -1 != HttpPairList::find(key);
}

inline int HttpPairList::find(const std::string& key) const
{
	for (int index = 0; index < (int) pairs_.size(); index++)
	{
		if (mxcore::StringUtil::equalNoCase(pairs_[index].first, key))
		{
			return index;
		}
	}

	return -1;
}

inline void HttpPairList::add(const HttpPair& kv)
{
	pairs_.push_back(kv);
}

inline void HttpPairList::add(const std::string& key, const std::string& value)
{
	pairs_.push_back(std::make_pair(key, value));
}

inline bool HttpPairList::remove(const std::string& key)
{
	int index = find(key);

	if (-1 != index)
	{
		pairs_.erase(pairs_.begin() + index);
	}

	return -1 != index;
}

} // namespace mxhttp

#endif /* __MXHTTP_HTTPPAIRLIST_H__ */
