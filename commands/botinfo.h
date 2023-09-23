#pragma once
#include <client/client.hpp>
#include <client/command.hpp>

namespace commands {
    void botinfo(Client&, cmd_t);
    void botinfo_create(Command&);
}
