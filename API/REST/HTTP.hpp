#pragma once
#include <boost/beast.hpp>
#include <functional>
#include <string_view>
#include <string>
#include <util/util.hpp>
#include <boost/asio/io_context.hpp>

namespace REST {
    namespace beast = boost::beast;     // from <boost/beast.hpp>
    namespace http = beast::http;       // from <boost/beast/http.hpp>
    namespace net = boost::asio;        // from <boost/asio.hpp>
    using tcp = net::ip::tcp; 

    const std::string default_port_http = "80";
    const char default_path[] = "/";

    class http_req {
        protected:
            typedef http::verb http_method;
            
            net::io_context ioc;
        public:
            beast::tcp_stream stream;
            http::request<http::string_body> request;

            std::string_view port;
            net::ip::tcp::resolver resolver;
            net::ip::tcp::resolver::query query;


            http_req(
                    const std::string& _host,
                    std::string _path,
                    http::verb method,
                    const std::string& _port = default_port_http);
            http_req(http_req&&) = default;
            http_req(const http_req&) = default;
            ~http_req();

            bool send(http::response<http::dynamic_body>& resp);
            bool send(http::response<http::dynamic_body>& resp, beast::flat_buffer& buffer);
            void set(http::field field, boost::string_view value) noexcept;
            void setTarget(boost::string_view str);
    };
}
