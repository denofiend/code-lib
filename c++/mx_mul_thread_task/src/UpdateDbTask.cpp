/*
 * UpdateDbTask.cpp
 *
 *  Created on: 2012-2-28
 *      Author: DenoFiend
 */

#include "UpdateDbTask.h"
#include <mxsql/SqlTransaction.h>
#include <mxsql/SqlConnection.h>
#include <mxsql/SqlException.h>
#include <mxcore/Number.h>

namespace mx_mul
{

UpdateDbTask::UpdateDbTask(mxsql::DataSource* datasource, int userId,
		const std::string& nickname) :
		datasource_(datasource), userId_(userId), nickname_(nickname)
{
}

UpdateDbTask::~UpdateDbTask()
{
}

void UpdateDbTask::run(void)
{

	if (!isNickNameExisits())
	{
		return;
	}

	if (!isTmpExisits())
	{
		nickname_ += mxcore::Integer(userId_).toString();
	}

	update();
}

}

bool mx_mul::UpdateDbTask::isNickNameExisits()
{
	bool ret = false;
	try
	{
		std::auto_ptr<mxsql::SqlConnection> connection(
				datasource_->getConnection());

		{

			std::string sql =
					"select `nickname` from `base_user_info` where `user_id` = ?";

			std::auto_ptr<mxsql::SqlPreparedStatement> stmt(
					connection->preparedStatement(sql));

			stmt->setInt(1, userId_);

			std::auto_ptr<mxsql::SqlResultSet> rs(stmt->executeQuery());

			if (rs->next())
			{
				nickname_ = rs->getString(1);
			}
			else
			{
				return false;
			}

		}

		{

			std::string sql1 =
					"select `user_id` from `base_user_info` where `nickname` = ?";

			std::auto_ptr<mxsql::SqlPreparedStatement> stmt1(
					connection->preparedStatement(sql1));

			stmt1->setString(1, nickname_);

			std::auto_ptr<mxsql::SqlResultSet> rs1(stmt1->executeQuery());
			while (rs1->next())
			{
				if (userId_ != rs1->getInt(1))
				{
					ret = true;
					break;
				}
			}

		}

	} catch (mxsql::SqlException & e)
	{
		printf(">>ERROR %d %s\n", e.getErrorCode(), e.getMessage().c_str());
	}
	return ret;
}

bool mx_mul::UpdateDbTask::isTmpExisits()
{
	bool ret = false;

	try
	{
		std::auto_ptr<mxsql::SqlConnection> connection(
				datasource_->getConnection());

		std::string sql =
				"select `nickname` from `tmp_nickname_unique` where `user_id` = ?";

		std::auto_ptr<mxsql::SqlPreparedStatement> stmt(
				connection->preparedStatement(sql));

		stmt->setInt(1, userId_);

		std::auto_ptr<mxsql::SqlResultSet> rs(stmt->executeQuery());

		if (rs->next())
		{
			ret = true;
			nickname_ = rs->getString(1);
		}

	} catch (mxsql::SqlException & e)
	{
		printf(">>ERROR %d %s\n", e.getErrorCode(), e.getMessage().c_str());
	}

	return ret;
}

int mx_mul::UpdateDbTask::update()
{

	int ret = -1;
	try
	{
		std::auto_ptr<mxsql::SqlConnection> connection(
				datasource_->getConnection());

		std::auto_ptr<mxsql::SqlTransaction> trans(
				connection->beginTransaction());

		std::string sql =
				"update `base_user_info` set `nickname`= ? where `user_id`=?";

		printf("[DEBUG] update userId(%d) nickname(%s)\n", userId_,
				nickname_.c_str());
		std::auto_ptr<mxsql::SqlPreparedStatement> stmt(
				connection->preparedStatement(sql));

		stmt->setString(1, nickname_);
		stmt->setInt(2, userId_);

		ret = stmt->executeUpdate();

		trans->commit();
	} catch (mxsql::SqlException & e)
	{
		printf(">>ERROR %d %s\n", e.getErrorCode(), e.getMessage().c_str());
	}

	return ret;
}

/* namespace mx_mul */
