AUTOMAKE_OPTIONS=foreign
lib_LTLIBRARIES=libmxasio.la
LDFLAGS=-fPIC -shared
LIBS=-levent
INCLUDES= -I../include -I../../mxcore/include -I/usr/local/include/libevent \
	-I/usr/include/libevent -I/usr/local/include
libmxasio_la_LDFLAGS = -version-info @lt_major@:@lt_revision@:@lt_age@
#libmxasio_la_LIBADD = ../../mxcore/src/libmxcore.la
libmxasioincludedir=$(includedir)/mxlib/mxasio
libmxasioinclude_HEADERS=../include/mxasio/AsyncTcpApplication.h \
	../include/mxasio/AsyncTcpServer.h \
	../include/mxasio/ChannelId.h \
	../include/mxasio/ChannelMessage.h \
	../include/mxasio/IOApplication.h \
	../include/mxasio/IOService.h
	
	
pkgincludedir= $(includedir)/mxlib/mxasio/libevent
pkginclude_HEADERS= ../include/mxasio/libevent/ChannelContext.h \
	../include/mxasio/libevent/IOServiceLibEvent.h \
	../include/mxasio/libevent/Wakeuper.h
	
libmxasio_la_SOURCES=AsyncTcpServer.cpp \
	ChannelContext.cpp \
	ChannelId.cpp \
	ChannelMessage.cpp \
	IOService.cpp \
	IOServiceLibEvent.cpp \
	Wakeuper.cpp
