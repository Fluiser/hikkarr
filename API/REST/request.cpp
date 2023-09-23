//
// Created by fluiser on 06.11.2022.
//

#include <stdint.h>
#include "request.hpp"
#include <util/util.hpp>

namespace REST {
	request::request(std::string _host,
		std::string _path,
		http::verb method,
		bool isHttps)
	{
		this->isHttps = isHttps;
		if (isHttps)
			new (&this->_req.https) https_req(_host, _path, method);
		else
			new (&this->_req.http) http_req(_host, _path, method);
	}

	request::request(const std::string& url, http::verb method)
	{
		if (url.size() < 5 || url.size() == 0)
			stack_log("URL size is so small; size=", url.size());
		std::string host;
		std::string path;
		int startHost = url[4] == 's' ? 8 : 7;
		int endHost = url.size();
		for (auto it = url.begin() + startHost; it != url.end(); ++it)
		{
			if (*it == '/')
			{
				endHost = it - url.begin();
				break;
			}
		}
		host.append(url.c_str() + startHost, endHost - startHost);
		if (url.size() - endHost)
			path.append(url.c_str() + endHost, url.size() - endHost);
		else path = "/";
		if (startHost == 8)
		{
			new (&this->_req.https) https_req(host, path, method);
			this->isHttps = true;
		}
		else 
			new (&this->_req.http) http_req(host, path, method);
	}

	request::~request()
	{
		if (isHttps)
			this->_req.https.~https_req();
		else
			this->_req.http.~http_req();
	}

	http::request<http::string_body>& request::get_request()
	{
		if (isHttps) return this->_req.https.request;
		else return this->_req.http.request;
	}

	request::response_d request::sendf()
	{
		response_d r{};
		if (isHttps) this->_req.https.send(r.body, r.buffer);
		else this->_req.http.send(r.body, r.buffer);
		return r;
	}

	//template<class T = http::dynamic_body> 
	//http::response<T> request::send()
	//{
	//	http::response<T> resp;
	//	if (isHttps) this->_req.https.send(resp);
	//	else this->_req.http.send(resp);
	//	return resp;
	//}
}