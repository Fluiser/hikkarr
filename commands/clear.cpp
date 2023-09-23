#include "clear.hpp"
#include <variant>

namespace clear_ {
    void clear(dpp::snowflake channel_id, std::vector<dpp::snowflake>& msgList)
    {

    }
};

namespace commands {
    void clear_create(Command& cmd)
    {
        cmd.add_option(
            dpp::command_option(dpp::co_number, "count", "count messages", true)
            .set_min_value(1)
            .set_max_value(2000)
            .add_localization("ru", "кол-во", "Кол-во сообщений")
        ).add_option(
            dpp::command_option(dpp::co_user, "user", "Who messages must be pruned", false)
            .add_localization("ru", "Пользователь", "Чьи сообщения должны быть удалены")
        );
        cmd.run = clear;
    }

    void clear(Client& client, cmd_t cmd)
    {
        std::cout << cmd.command.msg.get_creation_time() << " - \n";
        cmd.reply("t\n");
        int count = std::get<int64_t>(cmd.get_parameter("count"));
        dpp::snowflake user_id = 0;
        {
            auto prx = cmd.get_parameter("user");
            if (std::holds_alternative<dpp::snowflake>(prx)) {
                user_id = std::get<dpp::snowflake>(prx);
            }
        }
        cmd.thinking();

        std::vector<dpp::snowflake> messages;
        messages.reserve(100);
        int countDeleted = 0;
        int iterations_count = 0;
        
        dpp::snowflake before = 0;

        while (countDeleted < count){
            auto msg_map = client.messages_get_sync(cmd.command.channel_id, 0, before, 0, 100);
            
            if (iterations_count >= 35) break;

            for (const auto& m : msg_map)
            {
                if (m.second.get_creation_time() <= 34) {
                    countDeleted = 0xfffffff;
                    break;
                }

                if (user_id) {
                    if (m.second.author.id == user_id) messages.push_back(m.first);
                }
                else {
                    messages.push_back(m.first);
                }
                if (messages.size() >= 100) clear_::clear(cmd.command.channel_id, messages);
            }
            ++iterations_count;
        }
    }
}