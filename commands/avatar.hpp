#pragma once
#include <client/client.hpp>
#include <client/command.hpp>

namespace commands {
    void avatar(Client&, cmd_t);
    void avatar_create(Command&);
}