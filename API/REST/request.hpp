//
// Created by fluiser on 06.11.2022.
//

#ifndef HIKKARR_REQUEST_H
#define HIKKARR_REQUEST_H

#include "HTTPS.hpp"
#include "HTTP.hpp"

namespace REST {
    class request {
        bool isHttps = false;
        union __CLASS_HTTPS_HTTP_PROXY {
            https_req https;
            http_req http;
            __CLASS_HTTPS_HTTP_PROXY()
            {

            }
            ~__CLASS_HTTPS_HTTP_PROXY()
            {

            }
        } _req;

    public:

        struct response_d {
            beast::flat_buffer buffer;
            http::response<http::dynamic_body> body;
        };

        http::request<http::string_body>& get_request();

        request(std::string _host,
                std::string _path,
                http::verb method,
                bool isHttps = false);
        request(const std::string&, http::verb method = http::verb::get);
        http::response<http::dynamic_body> send();
        response_d sendf();
        //http::response<http::dynamic_body> send(beast::flat_buffer&);

        ~request();
    };
}


#endif //HIKKARR_REQUEST_H
