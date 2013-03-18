/*
 * UpdateDbTask.h
 *
 *  Created on: 2012-2-28
 *      Author: DenoFiend
 */

#ifndef UPDATEDBTASK_H_
#define UPDATEDBTASK_H_

#include <mxcore/Runnable.h>
#include <mxsql/DataSource.h>

namespace mx_mul
{

class UpdateDbTask: public mxcore::Runnable
{
public:
	UpdateDbTask(mxsql::DataSource* datasource, int userId,
			const std::string& nickname);
	virtual ~UpdateDbTask();
	virtual void run(void);

private:

	bool isNickNameExisits();

	bool isTmpExisits();

	int update();

	mxsql::DataSource* datasource_;
	int userId_;
	std::string nickname_;
};

} /* namespace mx_mul */
#endif /* UPDATEDBTASK_H_ */
