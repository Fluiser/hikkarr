#include "HTTP.hpp"
#pragma once

namespace REST {
    http_req::http_req(
        const std::string& _host,
        std::string _path,
        http::verb method,
        const std::string& _port):
            ioc(),
            resolver(ioc),
            stream(ioc),
            request{method, _path.data(), 11},
            port(_port),
            query(_host, _port)
    {
        this->set(http::field::user_agent, "</fluiser>");
        this->set(http::field::host, _host);
    }
    
    bool http_req::send(http::response<http::dynamic_body>& response) 
    {
        beast::flat_buffer buff;
        return this->send(response, buff);
    }

    bool http_req::send(http::response<http::dynamic_body>& response, beast::flat_buffer& buffer)
    {
        tcp::resolver resolver(ioc);
        try {
            tcp::resolver::iterator iter = resolver.resolve(query);
            stream.connect(iter->endpoint());
            this->set(http::field::host, query.host_name());


            http::write(stream, request);
            http::read(stream, buffer, response);
        }
        catch(const std::exception& e)
        {
            DEB_LOG << "err:" << e.what() << '\n';
            return false;
        }
        return true;
    }


    inline void http_req::set(http::field field, boost::string_view value) noexcept
    {
        request.set(field, value);
    }

    inline void http_req::setTarget(boost::string_view str)
    {
        request.target(str);
    }

    http_req::~http_req()
    {
        stream.close();
    }
}
