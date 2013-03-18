/*
 * SyncTask.h
 *
 *  Created on: 2013-3-15
 *      Author: DenoFiend
 */

#ifndef SYNCTASK_H_
#define SYNCTASK_H_

#include <mxcore/Runnable.h>
#include <mxsql/DataSource.h>

#include "sync/TaskBean.h"

namespace mx_mul
{

class SyncTask: public mxcore::Runnable
{

public:

	SyncTask(mxsql::DataSource* datasource, const std::string&syncUri);

	virtual ~SyncTask();

	virtual void run(void);

private:

	mxsql::DataSource* datasource_;
	std::string syncUri_;

private:
	TaskBean getOneTask();

	bool delTask(uint32_t queue_id);

};

} /* namespace mx_user_api */
#endif /* SYNCTASK_H_ */
