#--------------------------------------------------------------------
CC = gcc
LINKER = $(CC)

RM = /bin/rm -f

OBJS = AsyncEchoServer.o

PROGRAM = echoserver

CLEANFILES = core core.* *.core *.o 
MXLIB_ROOT = ../../..
INCLUDES = -I${MXLIB_ROOT}/mxcore/include -I${MXLIB_ROOT}/mxasio/include \
	-I/usr/local/include/libevent
	
LIBS = -lmxcore -lmxasio -levent
LDFLAGS = -L${MXLIB_ROOT}/mxcore -L${MXLIB_ROOT}/mxasio

CXXFLAGS = -g ${INCLUDES} -DMEM_DEBUG
#--------------------------------------------------------------------

all: ${PROGRAM}
	
${PROGRAM}: ${OBJS}
	$(CXX) -g -o ${PROGRAM} $(OBJS) $(LIBS) ${LDFLAGS}
	
clean:
	$(RM) $(CLEANFILES) ${PROGRAM}
#--------------------------------------------------------------------