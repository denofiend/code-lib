/*
 * HttpConfigurationXml.cpp
 *
 *  Created on: 2011-3-9
 *      Author: zhaojiangang
 */

#include <mxcore/LoggerFactory.h>
#include <mxhttp/HttpConfigurationXml.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

HttpConfigurationXml::HttpConfigurationXml(const std::string& filename) :
	filename_(filename)
{
}

HttpConfigurationXml::~HttpConfigurationXml(void)
{
}

static void parseLoggers(HttpConfig& baseConfig, mxcore::XmlElement& baseElt,
		const std::string& filepath) throw (mxcore::Exception&)
{
	mxcore::XmlElement loggersElt = baseElt.getFirstChildByName("loggers");

	if (loggersElt.isNull())
	{
		return;
	}

	mxcore::XmlElement loggerElt = loggersElt.getFirstChildByName("logger");

	while (!loggerElt.isNull())
	{
		mxcore::LoggerFactory::LoggerConfig loggerConfig;

		loggerConfig.name_ = loggerElt.getAttrString("name");
		loggerConfig.filepath_ = loggerElt.getAttrString("filepath");
		try
		{
			loggerConfig.bufferSize_ = mxcore::UShort::fromString(
					loggerElt.getAttrString("bufferSize"));
		} catch (mxcore::NumberFormatException& e)
		{
			THROW2(mxcore::Exception, std::string("File: [")
					+ filepath
					+ "], Invalid root.base.loggers.logger.bufferSize: ["
					+ loggerElt.getAttrString("bufferSize") + "]");
		}
		try
		{
			loggerConfig.level_ = mxcore::Integer::fromString(
					loggerElt.getAttrString("level"));
		} catch (mxcore::NumberFormatException& e)
		{
			THROW2(mxcore::Exception, std::string("File: [")
					+ filepath
					+ "], Invalid root.base.loggers.logger.level: ["
					+ loggerElt.getAttrString("bufferSize") + "]");
		}
		baseConfig.loggerList_.push_back(loggerConfig);
		loggerElt = loggerElt.getNextSiblingByName("logger");
	}
}

void HttpConfigurationXml::parseConfig(HttpConfig& baseConfig,
		std::list<VirtualServerConfig>& vsList) throw (mxcore::Exception&)
{
	mxcore::XmlDoc doc;
	try
	{
		doc.parseFile(filename_.c_str());
		mxcore::XmlElement rootElt = doc.getRootElement();

		if (rootElt.isNull())
		{
			THROW2(mxcore::Exception, std::string("File: [")
					+ filename_
					+ "], Not found root element");
		}

		mxcore::XmlElement baseElt = rootElt.getFirstChildByName("base");

		if (baseElt.isNull())
		{
			THROW2(mxcore::Exception, std::string("File: [")
					+ filename_
					+ "], Not found root.base element");
		}

		parseBaseConfig(baseConfig, baseElt);

		mxcore::XmlElement includeElt = rootElt.getFirstChildByName("include");

		while (!includeElt.isNull())
		{
			VirtualServerConfig vsConfig;
			parseIncludeVs(vsConfig, includeElt.getAttrString("filename"));
			vsList.push_back(vsConfig);
			includeElt = includeElt.getNextSiblingByName("include");
		}

		mxcore::XmlElement vsElt =
				rootElt.getFirstChildByName("virtual-server");

		if (vsElt.isNull())
		{
			return;
		}

		do
		{
			VirtualServerConfig vsConfig;
			parseVsConfig(vsConfig, vsElt, filename_);
			vsList.push_back(vsConfig);
			vsElt = vsElt.getNextSiblingByName("virtual-server");
		} while (!vsElt.isNull());

	} catch (mxcore::XmlException& e)
	{
		throw e;
	}
}

static void parseListen(mxcore::SocketAddress& address,
		mxcore::XmlElement& listenElt, const std::string& filepath)
		throw (mxcore::Exception&)
{
	std::string ip = listenElt.getAttrString("ip");
	std::string portStr = listenElt.getAttrString("port");

	if (ip.empty())
	{
		ip = "0.0.0.0";
	}

	if (portStr.empty())
	{
		portStr = "80";
	}

	try
	{
		address.set(ip, AF_INET, mxcore::Short::fromString(portStr));
	} catch (mxcore::NumberFormatException& e)
	{
		THROW2(mxcore::Exception, std::string("File: [")
				+ filepath
				+ "], Invalid root.base.listn.port: ["
				+ portStr + "]");
	} catch (mxcore::UnknownHostException& e)
	{
		THROW2(mxcore::Exception, std::string("File: [")
				+ filepath
				+ "], Unknown root.base.listen.ip: ["
				+ ip + "]");
	}
}

static void parseListens(HttpConfig& baseConfig, mxcore::XmlElement& baseElt,
		const std::string& filepath) throw (mxcore::Exception&)
{
	mxcore::XmlElement listenElt = baseElt.getFirstChildByName("listen");

	if (listenElt.isNull())
	{
		mxcore::SocketAddress address;
		address.setPort(80);
		baseConfig.listenList_.push_back(address);
	}
	else
	{
		do
		{
			mxcore::SocketAddress address;
			parseListen(address, listenElt, filepath);
			baseConfig.listenList_.push_back(address);
			listenElt = listenElt.getNextSiblingByName("listen");
		} while (!listenElt.isNull());
	}
}

static void parseUsername(HttpConfig& baseConfig, mxcore::XmlElement& baseElt,
		const std::string& filepath) throw (mxcore::Exception&)
{
	mxcore::XmlElement usernameElt = baseElt.getFirstChildByName("username");

	if (usernameElt.isNull())
	{
		THROW2(mxcore::Exception, std::string("File: [")
				+ filepath
				+ "], Not found root.base.username element");
	}

	baseConfig.username_ = usernameElt.getString();
}

static void parseConnection(HttpConfig& baseConfig,
		mxcore::XmlElement& baseElt, const std::string& filepath)
		throw (mxcore::Exception&)
{
	mxcore::XmlElement connectionElt =
			baseElt.getFirstChildByName("connection");

	if (connectionElt.isNull())
	{
		THROW2(mxcore::Exception, std::string("File: [")
				+ filepath
				+ "], Not found root.base.connection element");
	}

	std::string recvTimeoutStr = connectionElt.getAttrString("recvTimeout");

	if (!recvTimeoutStr.empty())
	{
		try
		{
			baseConfig.recvTimeout_ = mxcore::Integer::fromString(
					recvTimeoutStr);
		} catch (mxcore::NumberFormatException& e)
		{
			THROW2(mxcore::Exception, std::string("File: [")
					+ filepath
					+ "], Invalid root.base.connection.recvTimeout: ["
					+ recvTimeoutStr + "]");
		}
	}

	std::string maxPerProcessStr = connectionElt.getAttrString("maxPerProcess");

	if (!maxPerProcessStr.empty())
	{
		try
		{
			baseConfig.maxConnectionPerProcess_ = mxcore::UInteger::fromString(
					maxPerProcessStr);
		} catch (mxcore::NumberFormatException& e)
		{
			THROW2(mxcore::Exception, std::string("File: [")
					+ filepath
					+ "], Invalid root.base.connection.maxPerProcess: ["
					+ maxPerProcessStr + "]");
		}
	}

	std::string keepAliveTimeoutStr = connectionElt.getAttrString(
			"keepAliveTimeout");

	if (!keepAliveTimeoutStr.empty())
	{
		try
		{
			baseConfig.keepAliveTimeout_ = mxcore::Integer::fromString(
					keepAliveTimeoutStr);
		} catch (mxcore::NumberFormatException& e)
		{
			THROW2(mxcore::Exception, std::string("File: [")
					+ filepath
					+ "], Invalid root.base.connection.keepAliveTimeout: ["
					+ keepAliveTimeoutStr + "]");
		}
	}

	std::string forceKeepAliveStr = connectionElt.getAttrString(
			"forceKeepAlive");

	if (!forceKeepAliveStr.empty())
	{
		if (forceKeepAliveStr == "0")
		{
			baseConfig.forceKeepAvlie_ = false;
		}
		else if (forceKeepAliveStr == "1")
		{
			baseConfig.forceKeepAvlie_ = true;
		}
		else
		{
			THROW2(mxcore::Exception, std::string("File: [")
					+ filepath
					+ "], Invalid root.base.connection.forceKeepAlive: ["
					+ forceKeepAliveStr + "]");
		}
	}
}

static void parseParser(HttpConfig& baseConfig, mxcore::XmlElement& baseElt,
		const std::string& filepath) throw (mxcore::Exception&)
{
	mxcore::XmlElement parserElt = baseElt.getFirstChildByName("parser");

	if (parserElt.isNull())
	{
		THROW2(mxcore::Exception, std::string("File: [")
				+ filepath
				+ "], Not found root.base.parser element");
	}

	std::string bufferSizeStr = parserElt.getAttrString("bufferSize");

	if (!bufferSizeStr.empty())
	{
		try
		{
			baseConfig.bufferSize_
					= mxcore::UInteger::fromString(bufferSizeStr);
		} catch (mxcore::NumberFormatException& e)
		{
			THROW2(mxcore::Exception, std::string("File: [")
					+ filepath
					+ "], Invalid root.base.parser.bufferSize: ["
					+ bufferSizeStr + "]");
		}
	}

	baseConfig.tempRepository_ = parserElt.getAttrString("tmpPath");

	std::string maxContentLengthStr = parserElt.getAttrString(
			"maxContentLength");

	if (!maxContentLengthStr.empty())
	{
		try
		{
			baseConfig.maxContentLength_ = mxcore::Long::fromString(
					maxContentLengthStr);
		} catch (mxcore::NumberFormatException& e)
		{
			THROW2(mxcore::Exception, std::string("File: [")
					+ filepath
					+ "], Invalid root.base.parser.maxContentLength: ["
					+ maxContentLengthStr + "]");
		}
	}

	std::string cacheLimitSizeStr = parserElt.getAttrString("cacheLimitSize");

	if (!maxContentLengthStr.empty())
	{
		try
		{
			baseConfig.cachedLimitSize_ = mxcore::Long::fromString(
					cacheLimitSizeStr);
		} catch (mxcore::NumberFormatException& e)
		{
			THROW2(mxcore::Exception, std::string("File: [")
					+ filepath
					+ "], Invalid root.base.parser.cacheLimitSize: ["
					+ maxContentLengthStr + "]");
		}
	}
}

static void parseProcess(HttpConfig& baseConfig, mxcore::XmlElement& baseElt,
		const std::string& filepath) throw (mxcore::Exception&)
{
	mxcore::XmlElement processElt = baseElt.getFirstChildByName("process");

	if (!processElt.isNull())
	{
		try
		{
			baseConfig.subProcessNum_ = processElt.getAttrUInt("number");
		} catch (mxcore::NumberFormatException& e)
		{
			THROW2(mxcore::Exception, std::string("File: [")
					+ filepath
					+ "], Invalid root.base.process.number: ["
					+ processElt.getAttrString("number") + "]");
		}
	}
}

void HttpConfigurationXml::parseBaseConfig(HttpConfig& baseConfig,
		mxcore::XmlElement& baseElt) throw (mxcore::Exception&)
{
	parseUsername(baseConfig, baseElt, filename_);
	parseListens(baseConfig, baseElt, filename_);
	parseProcess(baseConfig, baseElt, filename_);
	parseConnection(baseConfig, baseElt, filename_);
	parseParser(baseConfig, baseElt, filename_);
	parseLoggers(baseConfig, baseElt, filename_);
}

static void parseListenForVs(mxcore::SocketAddress& address,
		mxcore::XmlElement& listenElt, const std::string& filepath)
		throw (mxcore::Exception&)
{
	std::string ip = listenElt.getAttrString("ip");
	std::string portStr = listenElt.getAttrString("port");

	if (ip.empty())
	{
		ip = "0.0.0.0";
	}

	if (portStr.empty())
	{
		portStr = "80";
	}

	try
	{
		address.set(ip, AF_INET, mxcore::Short::fromString(portStr));
	} catch (mxcore::NumberFormatException& e)
	{
		THROW2(mxcore::Exception, std::string("File: [")
				+ filepath
				+ "], Invalid root.base.listn.port: ["
				+ portStr + "]");
	} catch (mxcore::UnknownHostException& e)
	{
		THROW2(mxcore::Exception, std::string("File: [")
				+ filepath
				+ "], Unknown root.virtual-server.listen.ip: ["
				+ ip + "]");
	}
}

static void parsePaths(std::vector<std::string>& li,
		mxcore::XmlElement& pathsElt, const std::string& filepath)
{
	mxcore::XmlElement pathElt = pathsElt.getFirstChildByName("path");

	while (!pathElt.isNull())
	{
		std::string path = pathElt.getString();
		if (!path.empty())
		{
			li.push_back(path);
		}
		pathElt = pathElt.getNextSiblingByName("path");
	}
}

static void parseDownload(HttpConfiguration::VirtualServerConfig& vsConfig,
		mxcore::XmlElement& vsElt, const std::string& filepath)
		throw (mxcore::Exception&)
{
	mxcore::XmlElement downloadElt = vsElt.getFirstChildByName("download");
	vsConfig.enableFileDownload_ = false;

	if (downloadElt.isNull())
	{
		return;
	}

	std::string enableStr = downloadElt.getAttrString("enable");

	if (enableStr.empty() || enableStr == "0")
	{
		vsConfig.enableFileDownload_ = false;
	}
	else if (enableStr == "1")
	{
		vsConfig.enableFileDownload_ = true;
	}
	else
	{
		THROW2(mxcore::Exception
				, std::string("Invalid root.virtual-server.download.enable: [")
				+ enableStr + "]");
	}

	mxcore::XmlElement allowsElt = downloadElt.getFirstChildByName("allows");

	if (!allowsElt.isNull())
	{
		parsePaths(vsConfig.allowList_, allowsElt, filepath);
	}

	mxcore::XmlElement deninesElt = downloadElt.getFirstChildByName("denines");

	if (!deninesElt.isNull())
	{
		parsePaths(vsConfig.denineList_, deninesElt, filepath);
	}
}

void HttpConfigurationXml::parseVsConfig(VirtualServerConfig& vsConfig,
		mxcore::XmlElement& vsElt, const std::string& filepath)
		throw (mxcore::Exception&)
{
	mxcore::XmlElement domainElt = vsElt.getFirstChildByName("domain");

	if (domainElt.isNull())
	{
		THROW2 (mxcore::Exception, std::string("File: [")
				+ filepath + "], Not found root.virtual-server.domain.element");
	}

	vsConfig.domain_ = domainElt.getString();

	mxcore::XmlElement loggerElt = vsElt.getFirstChildByName("logger");

	mxcore::XmlElement rootDirElt = vsElt.getFirstChildByName("root-dir");

	vsConfig.rootDir_ = rootDirElt.getString();

	if (!loggerElt.isNull())
	{
		mxcore::LoggerFactory::LoggerConfig loggerConfig;
		loggerConfig.name_ = loggerElt.getAttrString("name");
		loggerConfig.filepath_ = loggerElt.getAttrString("filepath");
		try
		{
			loggerConfig.bufferSize_ = mxcore::UShort::fromString(
					loggerElt.getAttrString("bufferSize"));
		} catch (mxcore::NumberFormatException& e)
		{
			THROW2 (mxcore::Exception, std::string("File: [")
					+ filepath + "], Invalid root.virtual-server.logger.bufferSize: ["
					+ loggerElt.getAttrString("bufferSize") + "] for ["
					+ vsConfig.domain_ + "]");
		}
		try
		{
			loggerConfig.level_ = mxcore::Integer::fromString(
					loggerElt.getAttrString("level"));
		} catch (mxcore::NumberFormatException& e)
		{
			THROW2 (mxcore::Exception, std::string("File: [")
					+ filepath + "], Invalid root.virtual-server.logger.level: ["
					+ loggerElt.getAttrString("level") + "] for ["
					+ vsConfig.domain_ + "]");
		}
		vsConfig.loggerConfig_ = loggerConfig;
	}
	mxcore::XmlElement listenElt = vsElt.getFirstChildByName("listen");

	while (!listenElt.isNull())
	{
		mxcore::SocketAddress address;
		parseListenForVs(address, listenElt, filepath);
		vsConfig.bindList_.push_back(address);
		listenElt = listenElt.getNextSiblingByName("listen");
	}

	parseDownload(vsConfig, vsElt, filepath);

	mxcore::XmlElement servletElt = vsElt.getFirstChildByName("servlet");

	if (servletElt.isNull())
	{
		return;
	}

	do
	{
		ServletConfig servletConfig;
		parseServletConfig(servletConfig, servletElt, filepath);
		vsConfig.servlets_.push_back(servletConfig);
		servletElt = servletElt.getNextSiblingByName("servlet");
	} while (!servletElt.isNull());
}

void HttpConfigurationXml::parseIncludeVs(VirtualServerConfig& vsConfig,
		const std::string& filename) throw (mxcore::Exception&)
{
	std::string filepath(filename_);
	size_t pos = filepath.find_last_of('/');
	if (std::string::npos != pos)
	{
		filepath.erase(pos + 1);
		filepath += filename;
	}
	else
	{
		filepath = filename;
	}

	mxcore::XmlDoc doc;
	try
	{
		doc.parseFile(filepath.c_str());
		mxcore::XmlElement rootElt = doc.getRootElement();
		if (rootElt.isNull())
		{
			THROW2(mxcore::Exception, std::string("File: [")
					+ filepath
					+ "], not found root element");
		}
		parseVsConfig(vsConfig, rootElt, filepath);
	} catch (mxcore::XmlException& e)
	{
		throw e;
	}
}

static void parseParam(std::pair<std::string, std::string>& param,
		mxcore::XmlElement& paramElt)
{
	mxcore::XmlElement nameElt = paramElt.getFirstChildByName("name");
	mxcore::XmlElement valueElt = paramElt.getFirstChildByName("value");

	if (!nameElt.isNull())
	{
		param.first = nameElt.getString();
	}

	if (!valueElt.isNull())
	{
		param.second = valueElt.getString();
	}
}

void HttpConfigurationXml::parseServletConfig(ServletConfig& servletConfig,
		mxcore::XmlElement& servletElt, const std::string& filepath)
		throw (mxcore::Exception&)
{
	mxcore::XmlElement uriElt = servletElt.getFirstChildByName("uri");

	if (uriElt.isNull())
	{
		THROW2 (mxcore::Exception, std::string("File: [")
				+ filepath
				+ "], Not found root.base.virtual-server.servlet.uri");
	}

	servletConfig.uri_ = uriElt.getString();

	if (servletConfig.uri_.empty())
	{
		THROW2 (mxcore::Exception, std::string("File: [")
				+ filepath
				+ "], Not specific root.base.virtual-server.servlet.uri");
	}

	mxcore::XmlElement classElt = servletElt.getFirstChildByName("class");

	if (classElt.isNull())
	{
		THROW2 (mxcore::Exception, std::string("File: [")
				+ filepath
				+ "], Not found root.base.virtual-server.servlet.class");
	}

	servletConfig.class_ = classElt.getString();

	if (servletConfig.class_.empty())
	{
		THROW2 (mxcore::Exception, std::string("File: [")
				+ filepath
				+ "], Not specific root.base.virtual-server.servlet.class");
	}

	mxcore::XmlElement paramsElt = servletElt.getFirstChildByName("params");

	if (paramsElt.isNull())
	{
		return;
	}

	mxcore::XmlElement paramElt = paramsElt.getFirstChildByName("param");

	if (paramElt.isNull())
	{
		return;
	}

	do
	{
		std::pair<std::string, std::string> param;
		parseParam(param, paramElt);
		if (param.first.empty())
		{
			THROW2 (mxcore::Exception, std::string("File: [")
					+ filepath
					+ "], Not specific root.base.virtual-server.servlet.params.param.name");
		}
		servletConfig.params_[param.first].push_back(param.second);
		paramElt = paramElt.getNextSiblingByName("param");
	} while (!paramElt.isNull());
}

} // namespace mxhttp

