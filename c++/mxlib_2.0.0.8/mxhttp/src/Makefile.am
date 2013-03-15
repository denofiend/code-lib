AUTOMAKE_OPTIONS=foreign
lib_LTLIBRARIES=libmxhttp.la
LDFLAGS=-fPIC -shared
LIBS=-lpcre
INCLUDES= -I../include -I../../mxcore/include -I../../mxasio/include \
	-I/usr/include/libxml2 -I/usr/local/include/libxml2 \
	-I/usr/local/include
	
libmxhttp_la_LDFLAGS = -version-info @lt_major@:@lt_revision@:@lt_age@
#libmxhttp_la_LIBADD = ../../mxcore/src/libmxcore.la ../../mxasio/src/libmxasio.la
libmxhttpincludedir=$(includedir)/mxlib/mxhttp
libmxhttpinclude_HEADERS=../include/mxhttp/HttpBody.h \
	../include/mxhttp/HttpConfig.h \
	../include/mxhttp/HttpConfiguration.h \
	../include/mxhttp/HttpConfigurationXml.h \
	../include/mxhttp/HttpConnectionId.h \
	../include/mxhttp/HttpConstants.h \
	../include/mxhttp/HttpException.h \
	../include/mxhttp/HttpFileDownLoadServlet.h \
	../include/mxhttp/HttpListening.h \
	../include/mxhttp/HttpMessage.h \
	../include/mxhttp/HttpMsgParser.h \
	../include/mxhttp/HttpMultipartBody.h \
	../include/mxhttp/HttpPairList.h \
	../include/mxhttp/HttpParseException.h \
	../include/mxhttp/HttpResponser.h \
	../include/mxhttp/HttpServer.h \
	../include/mxhttp/HttpServlet.h \
	../include/mxhttp/HttpSimpleBody.h \
	../include/mxhttp/HttpSingleFileUploadServlet.h \
	../include/mxhttp/HttpStream.h \
	../include/mxhttp/HttpUtil.h \
	../include/mxhttp/HttpVirtualServer.h \
	../include/mxhttp/PcreUtil.h \
	../include/mxhttp/TempFileNameGenerator.h
	
pkgincludedir= $(includedir)/mxlib/mxhttp/impl
pkginclude_HEADERS= ../include/mxhttp/impl/HttpServerImpl.h \
	../include/mxhttp/impl/HttpServerConnection.h
	
libmxhttp_la_SOURCES=HttpBody.cpp \
	HttpConfig.cpp \
	HttpConfiguration.cpp \
	HttpConfigurationXml.cpp \
	HttpConstants.cpp \
	HttpException.cpp \
	HttpFileDownLoadServlet.cpp \
	HttpListening.cpp \
	HttpMessage.cpp \
	HttpMsgParser.cpp \
	HttpMultipartBody.cpp \
	HttpPairList.cpp \
	HttpParseException.cpp \
	HttpServer.cpp \
	HttpServerConnection.cpp \
	HttpServerImpl.cpp \
	HttpServlet.cpp \
	HttpSimpleBody.cpp \
	HttpSingleFileUploadServlet.cpp \
	HttpStream.cpp \
	HttpUtil.cpp \
	HttpVirtualServer.cpp \
	TempFileNameGenerator.cpp
	
