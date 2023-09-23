#pragma once
#include <client/client.hpp>
#include <client/command.hpp>

namespace commands {
void play(Client&, cmd_t);
void play_create(Command&);
}
