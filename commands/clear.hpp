#pragma once
#include <client/client.hpp>
#include <client/command.hpp>

namespace commands {
    void clear(Client&, cmd_t);
    void clear_create(Command&);
}
