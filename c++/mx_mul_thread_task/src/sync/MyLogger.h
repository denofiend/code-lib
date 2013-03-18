/*
 * MyLogger.h
 *
 *  Created on: 2013-3-18
 *      Author: DenoFiend
 */

#ifndef MYLOGGER_H_
#define MYLOGGER_H_


#include <mxcore/Logger.h>
#include <mxcore/LoggerFactory.h>


static inline mxcore::Logger& logger(void)
{
	return mxcore::LoggerFactory::getInstance().getLogger("sync");
}


#endif /* MYLOGGER_H_ */
