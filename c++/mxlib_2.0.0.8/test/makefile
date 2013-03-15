#--------------------------------------------------------------------
CC = gcc
LINKER = $(CC)

RM = /bin/rm -f

MXCORE_OBJS = mxcore/BytesUtilTest.o mxcore/SocketAddressTest.o \
	mxcore/SocketTest.o
	
MXSQL_OBJS = mxsql/MysqlTest.o mxsql/DataSourceTest.o

MXHTTP_OBJS = mxhttp/HttpBodyTest.o mxhttp/HttpParserTest.o \
	mxhttp/HttpServerTest.o mxhttp/HttpVirtualServerTest.o
MXASIO_OBJS = mxasio/IOServiceTest.o
	
OBJS = ${MXCORE_OBJS}
OBJS += ${MXSQL_OBJS}
OBJS += ${MXHTTP_OBJS}
OBJS += ${MXASIO_OBJS}
OBJS += main.o

PROGRAM = mxlibtest

CLEANFILES = core core.* *.core *.o 
MXLIB_ROOT = ..
INCLUDES = -I./ -I${MXLIB_ROOT}/mxcore/include -I${MXLIB_ROOT}/mxasio/include \
	-I/usr/local/include/libevent -I${MXLIB_ROOT}/mxsql/include \
	-I${MXLIB_ROOT}/mxhttp/include -I/usr/include/libxml2
	
LIBS = -lmxcore -lmxasio -lmxsql -lmxhttp
LDFLAGS = -L${MXLIB_ROOT}/mxcore -L${MXLIB_ROOT}/mxasio -L${MXLIB_ROOT}/mxsql \
	-L${MXLIB_ROOT}/mxhttp

CXXFLAGS = -g -DTEST ${INCLUDES}
#--------------------------------------------------------------------

all: ${PROGRAM}
	
${PROGRAM}: ${OBJS}
	$(CXX) -g -o ${PROGRAM} $(OBJS) $(LIBS) ${LDFLAGS}
	
clean:
	$(RM) $(CLEANFILES) ${PROGRAM} $(OBJS)
#--------------------------------------------------------------------