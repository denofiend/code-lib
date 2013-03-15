#--------------------------------------------------------------------
CC = gcc
LINKER = $(CC)

RM = /bin/rm -f

OBJS = src/main.o

PROGRAM = bin/mxhttpserv

CLEANFILES = core core.* *.core *.o 
MXLIB_ROOT = ..
INCLUDES = -I./ -I${MXLIB_ROOT}/mxcore/include -I${MXLIB_ROOT}/mxasio/include \
	-I/usr/local/include/libevent -I${MXLIB_ROOT}/mxsql/include \
	-I${MXLIB_ROOT}/mxhttp/include -I/usr/include/libxml2 -I/usr/local/include \
	-I/usr/local/include/libxml2
	
LIBS = -lmxcore -lmxasio -lmxhttp

CXXFLAGS = -g $(INCLUDES)
#--------------------------------------------------------------------

all: $(PROGRAM)
$(OBJS):
	$(CXX) $(CXXFLAGS) -c $< -o $(OBJS)
$(PROGRAM): $(OBJS)
	$(CXX) $(OBJS) $(LIBS) $(LDFLAGS) -o $(PROGRAM)
	
clean:
	$(RM) $(CLEANFILES) ${PROGRAM} ${OBJS}
#--------------------------------------------------------------------