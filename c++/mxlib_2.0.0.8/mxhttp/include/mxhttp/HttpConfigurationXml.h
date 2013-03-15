/*
 * HttpConfigurationXml.h
 *
 *  Created on: 2011-3-9
 *      Author: zhaojiangang
 */

#ifndef __MXHTTP_HTTPCONFIGURATIONXML_H__
#define __MXHTTP_HTTPCONFIGURATIONXML_H__

#include <mxcore/Xml.h>
#include <mxhttp/HttpConfiguration.h>

namespace mxhttp
{

class HttpConfigurationXml: public HttpConfiguration
{
public:
	HttpConfigurationXml(const std::string& filename);
	virtual ~HttpConfigurationXml(void);
protected:
	virtual void
	parseConfig(HttpConfig& baseConfig, std::list<VirtualServerConfig>& vsList)
			throw (mxcore::Exception&);
private:
	void parseBaseConfig(HttpConfig& baseConfig, mxcore::XmlElement& baseElt)
			throw (mxcore::Exception&);
	void
	parseVsConfig(VirtualServerConfig& vsConfig, mxcore::XmlElement& vsElt,
			const std::string& filepath) throw (mxcore::Exception&);

	void parseServletConfig(ServletConfig& servletConfig,
			mxcore::XmlElement& servletElt, const std::string& filepath)
			throw (mxcore::Exception&);

	void parseIncludeVs(VirtualServerConfig& vsConfig,
			const std::string& filename) throw (mxcore::Exception&);
private:
	std::string filename_;
};

} // mxhttp

#endif /* __MXHTTP_HTTPCONFIGURATIONXML_H__ */
