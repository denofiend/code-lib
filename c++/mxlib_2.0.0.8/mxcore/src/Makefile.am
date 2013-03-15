AUTOMAKE_OPTIONS=foreign
lib_LTLIBRARIES=libmxcore.la
LIBS=-lxml2 -lpthread
LDFLAGS=-fPIC -shared
INCLUDES = -I../include -I/usr/include/libxml2 \
	-I/usr/local/include -I/usr/local/include/libxml2

libmxcore_la_LDFLAGS = -version-info @lt_major@:@lt_revision@:@lt_age@
libmxcoreincludedir=$(includedir)/mxlib/mxcore
libmxcoreinclude_HEADERS=../include/mxcore/Assert.h \
	../include/mxcore/Atom.h \
	../include/mxcore/Base64.h \
	../include/mxcore/ByteBuffer.h \
	../include/mxcore/ByteOrder.h \
	../include/mxcore/CodeConvert.h \
	../include/mxcore/CodeConvertException.h \
	../include/mxcore/Condition.h \
	../include/mxcore/Counter.h \
	../include/mxcore/Exception.h \
	../include/mxcore/GenernicFactory.h \
	../include/mxcore/IniParser.h \
	../include/mxcore/IOException.h \
	../include/mxcore/IOUtil.h \
	../include/mxcore/Lock.h \
	../include/mxcore/Logger.h \
	../include/mxcore/LoggerFactory.h \
	../include/mxcore/Memory_.h \
	../include/mxcore/MemRecord.h \
	../include/mxcore/MessageBuffer.h \
	../include/mxcore/MutexLock.h \
	../include/mxcore/NotImplException.h \
	../include/mxcore/Number.h \
	../include/mxcore/NumberFormatException.h \
	../include/mxcore/Real.h \
	../include/mxcore/ReferenceCounter.h \
	../include/mxcore/Runnable.h \
	../include/mxcore/RuntimeException.h \
	../include/mxcore/ScopePtr.h \
	../include/mxcore/ServerSocket.h \
	../include/mxcore/SharedPtr.h \
	../include/mxcore/Socket.h \
	../include/mxcore/SocketAddress.h \
	../include/mxcore/StringUtil.h \
	../include/mxcore/SystemException.h \
	../include/mxcore/Thread.h \
	../include/mxcore/TimeUtil.h \
	../include/mxcore/UnknownHostException.h \
	../include/mxcore/UrlCodec.h \
	../include/mxcore/Xml.h \
	../include/mxcore/XmlException.h \
	../include/mxcore/Verify.h \
	../include/mxcore/SocketOps.h \
	../include/mxcore/SystemOps.h \
	../include/mxcore/ThreadOps.h
	
libmxcore_la_SOURCES= Base64.cpp \
	ByteBuffer.cpp \
	ByteOrder.cpp \
	CodeConvert.cpp \
	CodeConvertException.cpp \
	Condition.cpp \
	Counter.cpp \
	Exception.cpp \
	IniParser.cpp \
	IOException.cpp \
	IOUtil.cpp \
	Lock.cpp \
	Logger.cpp \
	LoggerFactory.cpp \
	Memory.cpp \
	MessageBuffer.cpp \
	MutexLock.cpp \
	NotImplException.cpp \
	Number.cpp \
	NumberFormatException.cpp \
	RuntimeException.cpp \
	ServerSocket.cpp \
	Socket.cpp \
	SocketAddress.cpp \
	SocketOps.cpp \
	StringUtil.cpp \
	SystemException.cpp \
	Thread.cpp \
	TimeUtil.cpp \
	UnknownHostException.cpp \
	UrlCodec.cpp \
	Xml.cpp \
	XmlException.cpp
