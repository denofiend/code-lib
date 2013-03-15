/*
 * HttpConfiguration.cpp
 *
 *  Created on: 2011-3-8
 *      Author: zhaojiangang
 */

#include <iostream>
#include <mxcore/SystemOps.h>
#include <mxcore/StringUtil.h>
#include <mxcore/GenernicFactory.h>
#include <mxhttp/HttpConfiguration.h>
#include <mxhttp/HttpFileDownLoadServlet.h>
#include <mxhttp/HttpSingleFileUploadServlet.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

static mxcore::RegisterInFactory<HttpServlet, HttpFileDownLoadServlet,
		std::string> regHttpFileDownloadServlet("mxhttp::FileDownloadServlet");

static mxcore::RegisterInFactory<HttpServlet, HttpSingleFileUploadServlet,
		std::string> regHttpSingleFileUploadServlet(
		"mxhttp::HttpSingleFileUploadServlet");

HttpConfiguration::ServletConfig::ServletConfig(void)
{
}

HttpConfiguration::ServletConfig::ServletConfig(const ServletConfig& src) :
	uri_(src.uri_), class_(src.class_), params_(src.params_)
{
}

HttpConfiguration::ServletConfig& HttpConfiguration::ServletConfig::operator =(
		const ServletConfig& other)
{
	if (this != &other)
	{
		uri_ = other.uri_;
		class_ = other.class_;
		params_ = other.params_;
	}
	return *this;
}

HttpConfiguration::ServletConfig::~ServletConfig(void)
{
}

HttpConfiguration::VirtualServerConfig::VirtualServerConfig(void) :
	enableFileDownload_(false)
{
}

HttpConfiguration::VirtualServerConfig::VirtualServerConfig(
		const VirtualServerConfig& src) :
	domain_(src.domain_), rootDir_(src.rootDir_), bindList_(src.bindList_),
			servlets_(src.servlets_),
			enableFileDownload_(src.enableFileDownload_),
			allowList_(src.allowList_), denineList_(src.denineList_),
			loggerConfig_(src.loggerConfig_)
{
}

HttpConfiguration::VirtualServerConfig& HttpConfiguration::VirtualServerConfig::operator =(
		const VirtualServerConfig& other)
{
	if (this != &other)
	{
		domain_ = other.domain_;
		rootDir_ = other.rootDir_;
		bindList_ = other.bindList_;
		servlets_ = other.servlets_;
		enableFileDownload_ = other.enableFileDownload_;
		allowList_ = other.allowList_;
		denineList_ = other.denineList_;
		loggerConfig_ = other.loggerConfig_;
	}
	return *this;
}

HttpConfiguration::VirtualServerConfig::~VirtualServerConfig(void)
{
}

HttpConfiguration::HttpConfiguration(void)
{
}

HttpConfiguration::~HttpConfiguration(void)
{
}

HttpServer* HttpConfiguration::buildInstance(void) throw (mxcore::Exception&)
{
	HttpConfig baseConfig;
	std::list<VirtualServerConfig> vsList;

	baseConfig.subProcessNum_ = 0;
	baseConfig.sendTimeout_ = 0;
	baseConfig.bufferSize_ = PARSER_BUFFER_SIZE;
	baseConfig.maxContentLength_ = PARSER_MAX_CONTENT_LENGTH;
	baseConfig.cachedLimitSize_ = PARSER_CACHE_LIMIT_SIZE;

	baseConfig.maxConnectionPerProcess_ = CONNECTION_MAX_PER_PROCESS;
	baseConfig.forceKeepAvlie_ = CONNECTION_FORCE_KEEP;
	baseConfig.keepAliveTimeout_ = CONNECTION_KEEP_ALIVE_TIMEOUT;
	baseConfig.recvTimeout_ = CONNECTION_TIMEOUT_RECV;
	parseConfig(baseConfig, vsList);
	checkValid(baseConfig, vsList);

	HttpServer* server = NULL;

	try
	{
		server = HttpServer::createInstance(baseConfig);

		for (std::list<VirtualServerConfig>::const_iterator it = vsList.begin(); it
				!= vsList.end(); it++)
		{
			const VirtualServerConfig& vsConfig = *it;

			mxcore::SharedPtr<HttpVirtualServer> vs = buildVirtualServer(*it);

			if (!server->addVirtualServer(vs, &vsConfig.bindList_))
			{
				THROW2(mxcore::Exception
						, std::string("Failed to add virtual server: [") + vs->getHost() + "]");
			}
		}

		return server;
	} catch (mxcore::Exception& e)
	{
		if (NULL != server)
		{
			delete server;
			server = NULL;
		}
		throw e;
	}
}

void HttpConfiguration::checkValid(HttpConfig& baseConfig,
		std::list<VirtualServerConfig>& vsList) const
		throw (mxcore::Exception&)
{
	checkBaseConfig(baseConfig);

	for (std::list<VirtualServerConfig>::const_iterator it = vsList.begin(); it
			!= vsList.end(); it++)
	{
		const VirtualServerConfig& vsConfig = *it;

		checkVSConfig(vsConfig);
	}
}

void HttpConfiguration::checkBaseConfig(const HttpConfig& baseConfig) const
		throw (mxcore::Exception&)
{
	if (baseConfig.bufferSize_ < 1024 || baseConfig.bufferSize_ > 40960)
	{
		THROW2 (mxcore::Exception
				, std::string("Invalid parser.bufferSize: [")
				+ mxcore::UInteger(baseConfig.bufferSize_).toString() + "] must great than 1024 and less than 40960");
	}

	if (baseConfig.tempRepository_.empty() || !mxos::isDirExists(
			baseConfig.tempRepository_.c_str()))
	{
		THROW2(mxcore::Exception
				, std::string("Invalid parser.tmpPath: [")
				+ baseConfig.tempRepository_ + "] dose not exists");
	}

	if (baseConfig.maxConnectionPerProcess_ < 1)
	{
		THROW2(mxcore::Exception
				, std::string("Invalid connection.maxPerProcess: [")
				+ mxcore::UInteger(baseConfig.maxConnectionPerProcess_).toString() + "] must larger than 0");
	}
}

void HttpConfiguration::checkServletConfig(const VirtualServerConfig& vsConfig,
		const ServletConfig& servletConfig) const throw (mxcore::Exception&)
{
	std::string uri = servletConfig.uri_;
	mxcore::StringUtil::trim(uri);

	if (uri.empty())
	{
		THROW2 (mxcore::Exception
				, std::string("Not specific uri for virtual server servlet: [")
				+ vsConfig.domain_ + "]");
	}

	uri = servletConfig.class_;
	mxcore::StringUtil::trim(uri);

	if (uri.empty())
	{
		THROW2 (mxcore::Exception
				, std::string("Not specific class for virtual server servlet: [")
				+ vsConfig.domain_ + "]");
	}

	HttpServlet* servlet =
			mxcore::GenernicFactory<HttpServlet>::getInstance().create(uri);

	if (NULL == servlet)
	{
		THROW2 (mxcore::Exception
				, std::string("Not found class: [")
				+ servletConfig.class_ + "] for virtual server servlet: ["
				+ vsConfig.domain_ + "]");
	}

	delete servlet;
}

void HttpConfiguration::checkVSConfig(const VirtualServerConfig& vsConfig) const
		throw (mxcore::Exception&)
{
	std::string domain = vsConfig.domain_;
	mxcore::StringUtil::trim(domain);

	if (domain.empty())
	{
		THROW2(mxcore::Exception
				, std::string("Not specific virtual-server.domain"));
	}

	if (!mxos::isDirExists(vsConfig.rootDir_.c_str()))
	{
		THROW2(mxcore::Exception
				, std::string("Invalid virtual-server.root-dir: [")
				+ vsConfig.rootDir_ + "] dose not exists");
	}

	for (std::vector<ServletConfig>::const_iterator it =
			vsConfig.servlets_.begin(); it != vsConfig.servlets_.end(); it++)
	{
		checkServletConfig(vsConfig, *it);
	}
}

mxcore::SharedPtr<HttpVirtualServer> HttpConfiguration::buildVirtualServer(
		const VirtualServerConfig& config) throw (mxcore::Exception&)
{
	mxcore::SharedPtr<HttpVirtualServer> vs = new HttpVirtualServer(
			config.domain_, config.rootDir_);

	vs->setLoggerConfig(config.loggerConfig_);

	if (config.enableFileDownload_)
	{
		mxcore::SharedPtr<HttpServlet> fileDownloadServlet =
				new HttpFileDownLoadServlet;

		std::map<std::string, std::vector<std::string> > params;
		params.insert(std::make_pair("allow", config.allowList_));
		params.insert(std::make_pair("denine", config.denineList_));

		fileDownloadServlet->setParams(params);

		fileDownloadServlet->init();
		vs->setFiledownloadSetvlet(fileDownloadServlet);
	}

	for (int i = 0; i < (int) config.servlets_.size(); i++)
	{
		const ServletConfig& servletConfig = config.servlets_[i];
		mxcore::SharedPtr<HttpServlet> servlet = buildServlet(servletConfig);

		if (!vs->addServlet(servletConfig.uri_, servlet))
		{
			THROW2(mxcore::Exception
					, std::string("Failed to add servlet: [") + servletConfig.uri_ + "]");
		}
	}

	return vs;
}

mxcore::SharedPtr<HttpServlet> HttpConfiguration::buildServlet(
		const ServletConfig& config) throw (mxcore::Exception&)
{
	mxcore::SharedPtr<HttpServlet> servlet = mxcore::GenernicFactory<
			HttpServlet>::getInstance().create(config.class_);

	if (servlet.isNull())
	{
		THROW2(mxcore::Exception
				, std::string("Not found the servlet class: [") + config.class_ + "]");
	}

	servlet->setParams(config.params_);
	return servlet;
}

} // namespace mxhttp
