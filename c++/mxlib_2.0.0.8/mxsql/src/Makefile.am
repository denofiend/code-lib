AUTOMAKE_OPTIONS=foreign
lib_LTLIBRARIES=libmxsql.la
LIBS=-lmysqlclient
LDFLAGS=-fPIC -shared
INCLUDES= -I../include -I../../mxcore/include -I/usr/local/include \
	-I/usr/local/mysql/include
libmxsql_la_LDFLAGS = -version-info @lt_major@:@lt_revision@:@lt_age@
#libmxsql_la_LIBADD = ../../mxcore/src/libmxcore.la

libmxsqlincludedir=$(includedir)/mxlib/mxsql
libmxsqlinclude_HEADERS=../include/mxsql/DataSource.h \
	../include/mxsql/DbConfig.h \
	../include/mxsql/SqlConnection.h \
	../include/mxsql/SqlConnectionFactory.h \
	../include/mxsql/SqlException.h \
	../include/mxsql/SqlResultSet.h \
	../include/mxsql/SqlStatement.h \
	../include/mxsql/SqlTransaction.h \
	../include/mxsql/ThreadedDataSource.h
	
pkgincludedir= $(includedir)/mxlib/mxsql/mysql
pkginclude_HEADERS= ../include/mxsql/mysql/MysqlConnection.h \
	../include/mxsql/mysql/MysqlConnectionFactory.h \
	../include/mxsql/mysql/MysqlPreparedResultSet.h \
	../include/mxsql/mysql/MysqlPreparedStatement.h \
	../include/mxsql/mysql/MysqlResultSet.h \
	../include/mxsql/mysql/MysqlStatement.h \
	../include/mxsql/mysql/MysqlTransaction.h

libmxsql_la_SOURCES=DataSource.cpp \
	DbConfig.cpp \
	MysqlConnection.cpp \
	MysqlConnectionFactory.cpp \
	MysqlPreparedResultSet.cpp \
	MysqlPreparedStatement.cpp \
	MysqlResultSet.cpp \
	MysqlStatement.cpp \
	MysqlTransaction.cpp \
	SqlException.cpp \
	ThreadedDataSource.cpp
