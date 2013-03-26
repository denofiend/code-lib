#--------------------------------------------------------------------
CC = gcc
LINKER = $(CC)

RM = /bin/rm -f

MAIN_ROOT = ./
	
	
MAIN_OBJS = ${MAIN_ROOT}/sync/HttpsClient.o \
	 ${MAIN_ROOT}/sync/SyncTask.o \
	 ${MAIN_ROOT}/sync/TaskBean.o \
	 ${MAIN_ROOT}/mul_task//TaskQueue.o \
	 ${MAIN_ROOT}/mul_task//TaskThread.o \
	 ${MAIN_ROOT}/sync/UpdateDbTask.o \
	 ${MAIN_ROOT}/sync/main.o \
	 
	 
	 
SYNC_OBJS = ${MAIN_OBJS}

SYNC = bin/mx_sync_serv

CLEANFILES = core core.* *.core *.o 
CXXFLAGS = -g -I${MAIN_ROOT}  \
	-I/usr/include/libxml2 -I/usr/local/mysql/include -I/usr/local/include/mxlib 
	
LDFLAGS = -L/usr/local/mysql/lib/mysql -L/usr/lib -L/usr/local/lib
LIBS = -lmxcore -lmxsql -lmxasio -lmxhttp \
	-ljson_linux-gcc-4.4.6_libmt -lmemcached -lxml2 \
	-levent -lmysqlclient -lcryptopp -lz -luuid -lcurl

CFLAGS = -I${MAIN_ROOT}
#--------------------------------------------------------------------

all: ${SYNC_OBJS}
	${CXX} -o ${SYNC} ${LIBS} ${LDFLAGS} ${SYNC_OBJS}

clean:
	$(RM) $(CLEANFILES) ${SYNC_OBJS} ${SYNC}
#--------------------------------------------------------------------
