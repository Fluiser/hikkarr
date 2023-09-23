#include "on_slashcommand.hpp"
#include <iostream>

namespace events {
    void on_slashcommand(Client& client, const dpp::slashcommand_t& event)
    {
        auto commandName = event.command.get_command_name();
        std::cout << "COMMAND: " << commandName << '\n';
        if(client.slashcommands.count(commandName) > 0) {
            client.slashcommands[commandName].run(client, event);
        }
    }
}
