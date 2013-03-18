/*
 * HttpsClient.cpp
 *
 *  Created on: 2012-3-21
 *      Author: DenoFiend
 */

#include "HttpsClient.h"
#include <curl/curl.h>
#include <iostream>
#include <mxcore/ByteBuffer.h>
#include <stdio.h>
using namespace std;

mxcore::ByteBuffer buffer(409600);

HttpsClient::HttpsClient() :
		curl_(NULL), revCode_(CURL_LAST)
{

}

HttpsClient::~HttpsClient()
{
}

static size_t revfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int revSize = size * nmemb;

	buffer.put(ptr, revSize);

	return revSize;
}
std::string HttpsClient::httpsPost(const std::string & url)
{
	if (NULL == curl_)
		curl_ = curl_easy_init();

	setHeaders();

	perform(url);

	buffer.flip();
	std::string ret;
	for (int i = 0; i < (int) buffer.getLimit(); ++i)
	{
		ret += (char) buffer.get(i);
	}

	buffer.clear();
	return ret;
}
std::string HttpsClient::httpsGet(const std::string & url)
{
	if (NULL == curl_)
		curl_ = curl_easy_init();

	setHeaders();

	perform(url);

	buffer.flip();
	std::string ret;
	for (int i = 0; i < (int) buffer.getLimit(); ++i)
	{
		ret += (char) buffer.get(i);
	}

	buffer.clear();
	return ret;
}

void HttpsClient::setHeaders()
{
	struct curl_slist *slist = NULL;
	for (std::map<std::string, std::string>::const_iterator iter =
			headers_.begin(); iter != headers_.end(); ++iter)
	{
		std::string mess = iter->first + ":" + iter->second;
		slist = curl_slist_append(slist, mess.c_str());
	}
	curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, slist);
}

std::string HttpsClient::httpGet(const std::string & url,
		const std::string & queryString)
{

	if (NULL == curl_)
		curl_ = curl_easy_init();
	std::string data = url + "?" + queryString;

	perform(data);

	buffer.flip();
	std::string ret;
	for (int i = 0; i < (int) buffer.getLimit(); ++i)
	{
		ret += (char) buffer.get(i);
	}
	buffer.clear();
	return ret;
}

std::string HttpsClient::httpPost(const std::string & url,
		const std::string queryString)
{
	if (NULL == curl_)
		curl_ = curl_easy_init();
	curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, queryString.c_str());

	perform(url);

	buffer.flip();
	std::string ret;
	for (int i = 0; i < (int) buffer.getLimit(); ++i)
	{
		ret += (char) buffer.get(i);
	}

	buffer.clear();
	return ret;
}

std::string HttpsClient::httpPost(const std::string & url,
		const std::string & queryString, const std::string & contentType)
{
	if (NULL == curl_)
		curl_ = curl_easy_init();

	curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, queryString.c_str());
	struct curl_slist *slist = NULL;

	char buf[1000];
	snprintf(buf, sizeof(buf), "Content-Type: %s", contentType.c_str());
	slist = curl_slist_append(slist, buf);

	curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, slist);

	perform(url);

	buffer.flip();
	std::string ret;
	for (int i = 0; i < (int) buffer.getLimit(); ++i)
	{
		ret += (char) buffer.get(i);
	}

	buffer.clear();
	return ret;
}

void HttpsClient::setHeader(const std::string & key, const std::string & val)
{
	headers_.insert(std::make_pair(key, val));
}

void HttpsClient::perform(const std::string & url)
{
	if (curl_)
	{
		curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, revfunc);
		curl_easy_setopt(curl_, CURLOPT_WRITEDATA, NULL);

#ifdef SKIP_PEER_VERIFICATION
		/*
		 * If you want to connect to a site who isn't using a certificate that is
		 * signed by one of the certs in the CA bundle you have, you can skip the
		 * verification of the server's certificate. This makes the connection
		 * A LOT LESS SECURE.
		 *
		 * If you have a CA cert for the server stored someplace else than in the
		 * default bundle, then the CURLOPT_CAPATH option might come handy for
		 * you.
		 */
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERFICATION
		/*
		 * If the site you're connecting to uses a different host name that what
		 * they have mentioned in their server certificate's commonName (or
		 * subjectAltName) fields, libcurl will refuse to connect. You can skip
		 * this check, but this will make the connection less secure.
		 */
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

		revCode_ = curl_easy_perform(curl_);

		/* always cleanup */
		curl_easy_cleanup(curl_);
	}
}

