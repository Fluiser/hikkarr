#include "play.hpp"
#include <client/embed.hpp>
#include <stdlib.h>
#include <util/util.hpp>
#include <thread>
#include <chrono>
#include "client/player/player.hpp"
#include <client/player/Sources/YTVideo.hpp>
#include <fstream>

namespace commands {
    void play_create(Command& cmd)
    {
        cmd.add_option(
            dpp::command_option(dpp::co_string, "id", "id", true)
                .set_max_length(100)
            );
        cmd.run = play;
    }

    void play(Client& client, cmd_t cmd)
    {
        auto* player = Music::Player::get_player(cmd.command.guild_id);
        if(!player) {
            player = Music::Player::create(client, cmd.command.guild_id, cmd.command.usr.id);
            if(!player) {
                cmd.reply(cmd.command.locale == "ru" ? "Не удалось подключиться" : "Can't connect to voice channel");
            }
        }


    }

}
