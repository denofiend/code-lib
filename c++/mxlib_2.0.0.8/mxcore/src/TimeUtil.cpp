/*
 * TimeUtil.cpp
 *
 *  Created on: 2011-3-10
 *      Author: zhaojiangang
 */

#include <mxcore/TimeUtil.h>
#include <mxcore/MemRecord.h>

namespace mxcore
{

std::string TimeUtil::toString(time_t dt)
{
	return toString(dt, "%Y-%m-%d %H:%M:%S");
}

std::string TimeUtil::toString(time_t dt, const std::string& fmt)
{
	char tmp[128 + 1] =
	{ 0 };
	struct tm tmT = getLocalTime(dt);
	::strftime(tmp, 128, fmt.c_str(), &tmT);
	return std::string(tmp);
}

bool TimeUtil::fromString(const std::string& str, const std::string& fmt,
		time_t& dt)
{
	struct tm tmT;
	const char* p = strptime(str.c_str(), fmt.c_str(), &tmT);
	if (NULL == p || *p != '\0')
	{
		return false;
	}

	tmT.tm_isdst = -1;
	dt = mktime(&tmT);
	return true;
}

} // namespace mxcore
