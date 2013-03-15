/*
 * HttpSingleFileUploadServlet.cpp
 *
 *  Created on: 2011-3-8
 *      Author: zhaojiangang
 */

#include <mxcore/Assert.h>
#include <mxcore/StringUtil.h>
#include <mxcore/SystemOps.h>
#include <mxhttp/HttpUtil.h>
#include <mxcore/LoggerFactory.h>
#include <mxhttp/HttpVirtualServer.h>
#include <mxhttp/HttpSingleFileUploadServlet.h>
#include <mxcore/GenernicFactory.h>
#include <mxcore/MemRecord.h>

namespace mxhttp
{

TimePartGenerator::TimePartGenerator(const std::string& prefix) :
	prefix_(prefix)
{
}

TimePartGenerator::~TimePartGenerator()
{
}

std::string TimePartGenerator::genPart(const HttpRequest& request)
{
	char tmp[32];
	::sprintf(tmp, "%u", (unsigned int) time(NULL));
	return prefix_ + tmp;
}

PidPartGenerator::PidPartGenerator(const std::string& prefix) :
	prefix_(prefix)
{
}

PidPartGenerator::~PidPartGenerator()
{
}

std::string PidPartGenerator::genPart(const HttpRequest& request)
{
	char tmp[32];
	::sprintf(tmp, "%d", (int) ::getpid());
	return prefix_ + tmp;
}

ClientIpPartGenerator::ClientIpPartGenerator(const std::string& prefix) :
	prefix_(prefix)
{
}

ClientIpPartGenerator::~ClientIpPartGenerator()
{
}

std::string ClientIpPartGenerator::genPart(const HttpRequest& request)
{
	std::string clientIp = request.getHeaders().getValue(
			HttpConstants::HEADER_X_FORWARD_FOR);

	if (clientIp.empty())
	{
		clientIp = request.getClientIp();
	}

	if (clientIp.empty())
	{
		clientIp = "0.0.0.0";
	}

	return prefix_ + clientIp;
}

StrPartGenerator::StrPartGenerator(const std::string& prefix) :
	prefix_(prefix)
{
}

StrPartGenerator::~StrPartGenerator()
{
}

std::string StrPartGenerator::genPart(const HttpRequest& request)
{
	return prefix_;
}

SeqPartGenerator::SeqPartGenerator(const std::string& prefix) :
	prefix_(prefix), sequence_(0)
{
}

SeqPartGenerator::~SeqPartGenerator()
{
}

std::string SeqPartGenerator::genPart(const HttpRequest& request)
{
	char tmp[32];
	::sprintf(tmp, "%u", sequence_);

	sequence_++;

	if (sequence_ >= 65535)
	{
		sequence_ = 0;
	}

	return prefix_ + tmp;
}

HttpUploadFilenameGenerator* HttpUploadFilenameGenerator::parse(
		const std::string& expressionStr)
{
	bool err = false;
	std::list<FilenamePartGenerator*> li;

	std::string expression(expressionStr);

	mxcore::StringUtil::trim(expression);

	if (expression.empty())
	{
		return NULL;
	}

	for (; !expression.empty();)
	{
		size_t pos = expression.find("${");

		if (std::string::npos == pos)
		{
			li.push_back(new StrPartGenerator(expression));
			break;
		}

		size_t pos1 = expression .find('}', pos);

		if (std::string::npos == pos1)
		{
			err = true;
			break;
		}

		std::string subExpression =
				expression.substr(pos + 2, pos1 - (pos + 2));

		if (subExpression.empty())
		{
			err = true;
			break;
		}

		if (mxcore::StringUtil::equalNoCase(subExpression, "time"))
		{
			li.push_back(new TimePartGenerator(expression.substr(0, pos)));
		}
		else if (mxcore::StringUtil::equalNoCase(subExpression, "pid"))
		{
			li.push_back(new PidPartGenerator(expression.substr(0, pos)));
		}
		else if (mxcore::StringUtil::equalNoCase(subExpression, "client_ip"))
		{
			li.push_back(new ClientIpPartGenerator(expression.substr(0, pos)));
		}
		else if (mxcore::StringUtil::equalNoCase(subExpression, "seq"))
		{
			li.push_back(new SeqPartGenerator(expression.substr(0, pos)));
		}
		else
		{
			err = true;
			break;
		}

		expression.erase(0, pos1 + 1);
		mxcore::StringUtil::trim(expression);
	}

	if (li.empty())
	{
		return NULL;
	}

	return new HttpUploadFilenameGeneratorImpl(li);
}

HttpUploadFilenameGeneratorImpl::HttpUploadFilenameGeneratorImpl(
		const std::list<FilenamePartGenerator*>& partGenList) :
	partGenList_(partGenList)
{
	MX_ASSERT(!partGenList.empty());
}

HttpUploadFilenameGeneratorImpl::~HttpUploadFilenameGeneratorImpl()
{
	for (std::list<FilenamePartGenerator*>::iterator it = partGenList_.begin(); it
			!= partGenList_.end(); it++)
	{
		delete *it;
	}
	partGenList_.clear();
}

std::string HttpUploadFilenameGeneratorImpl::getNextFilename(
		const HttpRequest& request)
{
	std::string fileName;

	for (std::list<FilenamePartGenerator*>::iterator it = partGenList_.begin(); it
			!= partGenList_.end(); it++)
	{
		fileName += (*it)->genPart(request);
	}

	return fileName;
}

HttpSingleFileUploadServlet::HttpSingleFileUploadServlet(void)
{
}

HttpSingleFileUploadServlet::~HttpSingleFileUploadServlet(void)
{
}

void HttpSingleFileUploadServlet::service(
		mxcore::SharedPtr<HttpVirtualServer> vs,
		mxcore::SharedPtr<HttpRequest> request, HttpResponser* responser)
{
	std::string filename = rootPath_;

	try
	{
		if (!request->isPost())
		{
			responser->response(
					request,
					HttpUtil::makeResponse(*request,
							HttpConstants::STATUS_METHOD_NOT_ALLOWED, NULL));

			return;
		}

		if (!checkBody(*request))
		{
			responser->response(
					request,
					HttpUtil::makeResponse(*request,
							HttpConstants::STATUS_LENGTH_REQUIRED, NULL));

			return;
		}

		HttpBody* body = request->getBody();

		filename += filenameGen_->getNextFilename(*request);

		if (body->isSimpleBody())
		{
			HttpUtil::writeToFile((HttpSimpleBody*) body, filename);
		}
		else
		{
			HttpUtil::writeToFile(
					((HttpMultipartBody*) body)->getFirstUploadFileItem(),
					filename);
		}
		vs->getLogger().info("Connection [%s] upload file: [%s]\n",
				request->getConnectionId().toString().c_str(), filename.c_str());

		responser->response(
				request,
				HttpUtil::makeResponse(*request, HttpConstants::STATUS_OK, NULL));
	} catch (mxcore::IOException& e)
	{
		vs->getLogger().error2(e.getErrorCode(),
				"Connection [%s] write file: [%s]: [%s]\n",
				request->getConnectionId().toString().c_str(),
				filename.c_str(), e.getMessage().c_str());

		responser->response(
				request,
				HttpUtil::makeResponse(*request,
						HttpConstants::STATUS_SERVER_ERROR, NULL));
	}
}

void HttpSingleFileUploadServlet::initImpl(void) throw (mxcore::Exception&)
{
	std::map<std::string, std::vector<std::string> >::iterator it =
			params_.find("path");

	if (it == params_.end())
	{
		THROW2(mxcore::Exception, "HttpSingleFileUploadServlet::initImpl: Not specific path param");
	}

	rootPath_ = it->second.front();

	if (!mxcore::StringUtil::endWith(rootPath_, "/"))
	{
		rootPath_ += "/";
	}

	if (!mxos::isDirExists(rootPath_.c_str()))
	{
		THROW2(mxcore::Exception, std::string("HttpSingleFileUploadServlet::initImpl: path [") + rootPath_ + "] dose not exists");
	}

	it = params_.find("filename");

	if (it == params_.end() || it->second.empty())
	{
		THROW2(mxcore::Exception, "HttpSingleFileUploadServlet::initImpl: Not specific filename param");
	}

	std::string filename = it->second.front();

	HttpUploadFilenameGenerator* filenameGenerator =
			HttpUploadFilenameGenerator::parse(filename);

	if (NULL == filenameGenerator)
	{
		THROW2(mxcore::Exception, std::string("HttpSingleFileUploadServlet::initImpl: Invalid filename param [") + filename +"]");
	}

	filenameGen_ = filenameGenerator;
	it = params_.find("logname");

	if (it != params_.end() && !it->second.empty())
	{
		loggerName_ = it->second.front();
	}
}

bool HttpSingleFileUploadServlet::checkBody(const HttpRequest& request) const
{
	HttpBody* body = request.getBody();
	if (NULL == body)
	{
		return false;
	}

	if (body->isSimpleBody())
	{
		return ((HttpSimpleBody*) body)->getContentLength() > 0;
	}
	else
	{
		HttpUploadFileItem* item =
				((HttpMultipartBody*) body)->getFirstUploadFileItem();

		return NULL != item && item->getContentLength() > 0;
	}
}

} // namespace mxhttp
