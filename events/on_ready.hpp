#pragma once
#include <client/client.hpp>

namespace events {
    void on_ready(Client&, const dpp::ready_t& event);
}