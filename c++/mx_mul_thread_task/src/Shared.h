/*
 * Shared.h
 *
 *  Created on: 2011-11-7
 *      Author: DenoFiend
 */

#ifndef FILEDATA_UTIL_SHARED_H_
#define FILEDATA_UTIL_SHARED_H_

#include <tr1/memory>
#include <mxcore/Runnable.h>
#include "TaskQueue.h"
#include "UpdateDbTask.h"


namespace mx_mul
{

typedef std::tr1::shared_ptr<mx_mul::TaskQueue> TaskQueuePtr;
typedef std::tr1::shared_ptr<mx_mul::UpdateDbTask> UpdateDbTaskPtr;

}

#endif /* FILEDATA_UTIL_SHARED_H_ */
