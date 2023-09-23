#include "HTTPS.h"
#include <iostream>
#pragma comment (lib, "crypt32");

namespace REST {
    https_req::https_req(
                std::string _host,
                std::string _path,
                http::verb method,
                const std::string& _port
            ) noexcept:
                resolver(ioc),
                query(_host, _port),
                request{method, _path, 11},
                ctx(ssl::context::sslv23_client),
                socket(ioc, ctx)
    {
        boost::system::error_code ec;
        ctx.set_default_verify_paths();
        if(ec) {
            std::cout << "err:" << ec.message() << '\n';
        }
        request.set(http::field::user_agent, "</fluiser>");
        request.set(http::field::host, _host);
    }


    bool https_req::send(  http::response<http::dynamic_body>& resp)
    {
        beast::flat_buffer buffer;
        return this->send(resp, buffer);
    }

    bool https_req::send(  http::response<http::dynamic_body>& resp,
                           beast::flat_buffer& buffer)
    {
        asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
        try {
            socket.lowest_layer().connect(*iter);

            socket.handshake(ssl::stream_base::handshake_type::client);
            http::write(socket, request);
            http::read(socket, buffer, resp);
        } catch(const std::exception& e) {
            std::cout << e.what();
            return false;
        }
        return true;
    }

//    void https_req::wait(beast::flat_buffer& buffer)
//    {
//        http::read(socket, buffer, response);
//    }

    void https_req::setTarget(boost::string_view str)
    {
        request.target(str);
    }

    inline void https_req::set(http::field field, boost::string_view value) noexcept
    {
        this->request.set(field, value);
    }

    https_req::~https_req()
    {

    }
}
