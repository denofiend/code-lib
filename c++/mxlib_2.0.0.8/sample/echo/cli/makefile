#--------------------------------------------------------------------
CC = gcc
LINKER = $(CC)

RM = /bin/rm -f

OBJS = SyncEchoClient.o

PROGRAM = echoclient

CLEANFILES = core core.* *.core *.o 
MXLIB_ROOT = ../../..
INCLUDES = -I${MXLIB_ROOT}/mxcore/include -I${MXLIB_ROOT}/mxasio/include
	
LIBS = -lmxcore
LDFLAGS = -L${MXLIB_ROOT}/mxcore

CXXFLAGS = -g ${INCLUDES} -DMEM_DEBUG
#--------------------------------------------------------------------

all: ${PROGRAM}
	
${PROGRAM}: ${OBJS}
	$(CXX) -g -o ${PROGRAM} $(OBJS) $(LIBS) ${LDFLAGS}
	
clean:
	$(RM) $(CLEANFILES) ${PROGRAM}
#--------------------------------------------------------------------