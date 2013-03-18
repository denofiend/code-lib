/*
 * HttpsClient.h
 *
 *  Created on: 2012-3-21
 *      Author: DenoFiend
 */

#ifndef HTTPSCLIENT_H_
#define HTTPSCLIENT_H_

#include <string>
#include <map>
#include <curl/curl.h>
#include <mxcore/MessageBuffer.h>

class HttpsClient
{
public:
	HttpsClient();
	virtual ~HttpsClient();

	std::string httpGet(const std::string& url, const std::string& queryString);


	std::string httpPost(const std::string&url, const std::string queryString);


	std::string httpsGet(const std::string&url);
	std::string httpsPost(const std::string&url);

	std::string httpPost(const std::string&url, const std::string &queryString,
			const std::string& contentType);

	void setHeader(const std::string&key, const std::string&val);

private:

	CURL *curl_;
	CURLcode revCode_;
	std::map<std::string, std::string> headers_;

private:
	void perform(const std::string& url);
	void setHeaders();
};

#endif /* HTTPSCLIENT_H_ */
