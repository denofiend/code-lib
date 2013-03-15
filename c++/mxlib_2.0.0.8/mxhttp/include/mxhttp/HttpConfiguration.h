/*
 * HttpConfiguration.h
 *
 *  Created on: 2011-3-8
 *      Author: zhaojiangang
 */

#ifndef __MXHTTP_HTTPCONFIGURATION_H__
#define __MXHTTP_HTTPCONFIGURATION_H__

#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <mxcore/Exception.h>
#include <mxhttp/HttpConfig.h>
#include <mxhttp/HttpServer.h>

namespace mxhttp
{

class HttpConfiguration
{
public:
	static const int CONNECTION_TIMEOUT_RECV = 5000;
	static const uint32_t CONNECTION_MAX_PER_PROCESS = 1000;
	static const int CONNECTION_KEEP_ALIVE_TIMEOUT = 5000;
	static const bool CONNECTION_FORCE_KEEP = false;

	static const uint32_t PARSER_BUFFER_SIZE = 4096;
	static const uint32_t PARSER_MAX_CONTENT_LENGTH = 409600;
	static const uint32_t PARSER_CACHE_LIMIT_SIZE = 40960;

	class ServletConfig
	{
	public:
		ServletConfig(void);
		ServletConfig(const ServletConfig&);
		ServletConfig& operator =(const ServletConfig&);
		~ServletConfig(void);
	public:
		std::string uri_;
		std::string class_;

		std::map<std::string, std::vector<std::string> > params_;
	};

	class VirtualServerConfig
	{
	public:
		VirtualServerConfig(void);
		VirtualServerConfig(const VirtualServerConfig&);
		VirtualServerConfig& operator =(const VirtualServerConfig&);
		~VirtualServerConfig(void);
	public:
		std::string domain_;
		std::string rootDir_;
		std::list<mxcore::SocketAddress> bindList_;
		std::vector<ServletConfig> servlets_;

		bool enableFileDownload_;
		std::vector<std::string> allowList_;
		std::vector<std::string> denineList_;

		mxcore::LoggerFactory::LoggerConfig loggerConfig_;
	};

	HttpConfiguration(void);
	virtual ~HttpConfiguration(void);

	HttpServer* buildInstance(void) throw (mxcore::Exception&);
protected:
	virtual void
	parseConfig(HttpConfig& baseConfig, std::list<VirtualServerConfig>& vsList)
			throw (mxcore::Exception&) = 0;
private:
	void checkValid(HttpConfig& baseConfig,
			std::list<VirtualServerConfig>& vsList) const
			throw (mxcore::Exception&);

	void checkBaseConfig(const HttpConfig& baseConfig) const
			throw (mxcore::Exception&);

	void checkServletConfig(const VirtualServerConfig& vsConfig,
			const ServletConfig& servletConfig) const
			throw (mxcore::Exception&);

	void checkVSConfig(const VirtualServerConfig& vsConfig) const
			throw (mxcore::Exception&);

	mxcore::SharedPtr<HttpVirtualServer> buildVirtualServer(
			const VirtualServerConfig& config) throw (mxcore::Exception&);

	mxcore::SharedPtr<HttpServlet> buildServlet(const ServletConfig& config)
			throw (mxcore::Exception&);
};

} // namespace mxhttp

#endif /* __MXHTTP_HTTPCONFIGURATION_H__ */
