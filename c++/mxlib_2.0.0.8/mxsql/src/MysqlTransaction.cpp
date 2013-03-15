/*
 *        Name: MysqlTransaction.cpp
 *
 *  Created on: 2011-2-1
 *      Author: zjg
 *		 Email: zjgzzz@gmail.com
 */

#include <mxcore/Assert.h>
#include <mxsql/mysql/MysqlConnection.h>
#include <mxsql/mysql/MysqlTransaction.h>
#include <mxcore/MemRecord.h>

namespace mxsql
{

MysqlTransaction::MysqlTransaction(MysqlConnection* connection) :
	connection_(connection), state_(BEGIN)
{
}

MysqlTransaction::~MysqlTransaction(void)
{
	if (BEGIN == state_)
	{
		connection_->rollback();
	}
}

void MysqlTransaction::commit(void) throw (SqlException&)
{
	MX_ASSERT(BEGIN == state_);
	state_ = COMMIT;
	connection_->commit();
}

void MysqlTransaction::rollback(void) throw (SqlException&)
{
	MX_ASSERT(ROLLBACK != state_);
	state_ = ROLLBACK;
	connection_->rollback();
}

} // namespace mxsql
