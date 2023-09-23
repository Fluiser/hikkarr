#include "on_ready.hpp"
#include <set>
#include <algorithm>

namespace events {

    void on_ready(Client& bot, const dpp::ready_t& event)
    {
        if(dpp::run_once<struct registerbot>()) {
            bot.global_commands_get([&bot](auto& _res) {{
                dpp::slashcommand_map commands = _res.template get<dpp::slashcommand_map>();
                std::set<std::string> notCreated; // Те, что не созданы у дс
                std::set<std::string> missing; //Тех, что нет у бота
                for(auto& p: bot.slashcommands) {
                    p.second.set_application_id(bot.me.id);
                    notCreated.insert(p.first);
                }

                std::cout << "Commands: " << commands.size() << "\n";
                for(const auto& pair: commands)
                {
                    bot.slashcommands[pair.second.name].id = pair.first;
                    notCreated.erase(pair.second.name);
                    missing.insert(pair.second.name);
                }
                for(const auto& cmdName: notCreated)
                {
                    std::cout << "CREATE COMMAND: " << cmdName << '\n'; 
                    auto& cmd = bot.slashcommands[cmdName];
                    bot.global_command_create(
                        cmd
                    );
                }
                {
                    std::list<std::string> namesForErase;
                    for(const auto& cmdName: missing)
                    {
                        if(bot.slashcommands.count(cmdName) > 0) {
                            namesForErase.emplace_back(cmdName);
                        }
                    }
                    for(const auto& str: namesForErase)
                        missing.erase(str);
                }
                for(const auto& cmdName: missing)
                {
                    auto snowflake = std::find_if(commands.begin(), commands.end(), [&cmdName](auto& pair) {
                        return (pair.second.name == cmdName);
                    });
                    std::cout << "DELETE COMMAND: " << cmdName << '\n';
                    bot.global_command_delete(snowflake->first);
                }
            }});

        }
    }
}
