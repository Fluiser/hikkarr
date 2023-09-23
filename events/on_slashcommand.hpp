#pragma once
#include <client/client.hpp>

namespace events {
    void on_slashcommand(Client&, const dpp::slashcommand_t&);
}