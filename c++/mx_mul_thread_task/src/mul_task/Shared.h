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
#include "sync/UpdateDbTask.h"
#include "sync/SyncTask.h"

namespace mx_mul
{

typedef std::tr1::shared_ptr<mx_mul::TaskQueue> TaskQueuePtr;
typedef std::tr1::shared_ptr<mx_mul::UpdateDbTask> UpdateDbTaskPtr;

typedef std::tr1::shared_ptr<mx_mul::SyncTask> SyncTaskPtr;
typedef std::tr1::shared_ptr<mx_mul::MinQidTask> MinQidTaskPtr;

}

#endif /* FILEDATA_UTIL_SHARED_H_ */
