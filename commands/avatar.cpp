#include "avatar.hpp"
#include <client/embed.hpp>

namespace commands {
    void avatar_create(Command& cmd)
    {
        cmd.add_localization("ru", "аватар", "Получить аватар указанного пользователя/свой");
        cmd.add_option(dpp::command_option(dpp::co_user, "user", "user", false).add_localization("ru", "user", "пользователь"));
        cmd.run = avatar;
    }

    void avatar(Client& client, cmd_t cmd)
    {
        auto user = cmd.get_parameter("user");

        dpp::embed embed = Embed();

        if(std::holds_alternative<std::monostate>(user)) {
            embed.set_image(cmd.command.get_issuing_user().get_avatar_url(4096));
        } else {
            embed.set_image(client.user_get_sync(std::get<dpp::snowflake>(user)).get_avatar_url(4096));
        }
        cmd.reply(dpp::message(cmd.command.channel_id, embed));
    }
}