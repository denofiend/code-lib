/*
 * HttpVirtualServer.h
 *
 *  Created on: 2011-3-3
 *      Author: zhaojiangang
 */

#ifndef __MXHTTP_HTTPVIRTUALSERVER_H__
#define __MXHTTP_HTTPVIRTUALSERVER_H__

#include <map>
#include <string>
#include <mxhttp/PcreUtil.h>
#include <mxhttp/HttpServlet.h>
#include <mxcore/LoggerFactory.h>

namespace mxhttp
{

class HttpServer;

class HttpVirtualServer
{
	friend class HttpServerImpl;
private:
	HttpVirtualServer(const HttpVirtualServer&);
	HttpVirtualServer& operator =(const HttpVirtualServer&);
public:
	HttpVirtualServer(const std::string& host, const std::string& rootDir);
	~HttpVirtualServer(void);

	mxcore::SharedPtr<HttpServlet> findServlet(const HttpRequest& request) const;
	bool addServlet(const std::string& path,
			mxcore::SharedPtr<HttpServlet> servlet);

	void setFiledownloadSetvlet(
			mxcore::SharedPtr<HttpServlet> filedownloadServlet);

	void setHost(const std::string& host);
	const std::string& getHost(void) const;

	void setRootDir(const std::string& rootDir);
	const std::string& getRootDir(void) const;

	HttpServer* getServer(void) const;

	std::string getAbsolutePath(const std::string& path) const;

	void setLoggerConfig(
			const mxcore::LoggerFactory::LoggerConfig& loggerConfig);
	const mxcore::LoggerFactory::LoggerConfig getLoggerConfig(void) const;

	mxcore::Logger& getLogger(void);
private:
	std::string host_;
	std::string rootDir_;

	typedef std::map<std::string, mxcore::SharedPtr<HttpServlet> > ServletMap;
	ServletMap servlets_;
	typedef std::map<pcre*, mxcore::SharedPtr<HttpServlet> > PcreMap;
	PcreMap pcres_;

	HttpServer* httpServer_;
	mxcore::SharedPtr<HttpServlet> filedownloadServlet_;

	mxcore::LoggerFactory::LoggerConfig loggerConfig_;
};

inline void HttpVirtualServer::setHost(const std::string& host)
{
	host_ = host;
}

inline const std::string& HttpVirtualServer::getHost(void) const
{
	return host_;
}

inline void HttpVirtualServer::setRootDir(const std::string& rootDir)
{
	rootDir_ = rootDir;
}

inline const std::string& HttpVirtualServer::getRootDir(void) const
{
	return rootDir_;
}

inline HttpServer* HttpVirtualServer::getServer(void) const
{
	return httpServer_;
}

inline void HttpVirtualServer::setLoggerConfig(
		const mxcore::LoggerFactory::LoggerConfig& loggerConfig)
{
	loggerConfig_ = loggerConfig;
}

inline const mxcore::LoggerFactory::LoggerConfig HttpVirtualServer::getLoggerConfig(
		void) const
{
	return loggerConfig_;
}

} // namespace mxttp

#endif /* __MXHTTP_HTTPVIRTUALSERVER_H__ */
