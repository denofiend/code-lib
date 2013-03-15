/*
 * TimeUtil.h
 *
 *  Created on: 2011-3-10
 *      Author: zhaojiangang
 */

#ifndef __MXCORE_TIMEUTIL_H__
#define __MXCORE_TIMEUTIL_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <time.h>
#include <string>
#include <sys/time.h>

namespace mxcore
{

class TimeUtil
{
	TimeUtil(void);
public:
	static struct tm getLocalTime(time_t dt);
	static std::string toString(time_t dt);
	static std::string toString(time_t dt, const std::string& fmt);
	static bool fromString(const std::string& str, const std::string& fmt,
			time_t& dt);
};

inline struct tm TimeUtil::getLocalTime(time_t dt)
{
	struct tm tmT;
#if defined(WIN32)
	struct tm* ptm = localtime(&dt);
	memcpy(&tmT, ptm, sizeof(struct tm));
#else
	localtime_r(&dt, &tmT);
#endif
	return tmT;
}

} // namespace mxcore

#endif /* TIMEUTIL_H_ */
