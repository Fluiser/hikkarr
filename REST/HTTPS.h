#pragma once
#include <boost/asio/ssl.hpp>

#include "HTTP.h"

namespace REST {
    namespace asio = boost::asio;
    namespace ssl = asio::ssl;

    const std::string default_port_https = "443";

    class https_req {
        ssl::context ctx;
        net::io_context ioc;
        ssl::stream<asio::ip::tcp::socket> socket;

        asio::ip::tcp::resolver resolver;
        asio::ip::tcp::resolver::query query;
    public:
        http::request<http::string_body> request;


        https_req() = default;
		https_req(
                std::string _host,
                std::string _path,
                http::verb method,
                const std::string& _port = default_port_https) noexcept;
		~https_req() noexcept;

        void set(http::field field, boost::string_view value) noexcept;
        void setTarget(boost::string_view str);
        bool send(  http::response<http::dynamic_body>& resp);
        bool send(  http::response<http::dynamic_body>& resp,
                    beast::flat_buffer& );

    };
}
