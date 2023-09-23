#include "test.hpp"
#include <client/embed.hpp>
#include <stdlib.h>
#include <util/util.hpp>
#include <thread>
#include <chrono>
#include "client/player/player.hpp"
#include <client/player/Sources/YTVideo.hpp>
#include <fstream>

#define DB(x) std::cout << x;

namespace commands {
    void test_create(Command& cmd)
    {
        cmd.add_option(
            dpp::command_option(dpp::co_string, "id", "id", true)
            .set_max_length(100)
        );
        cmd.run = test;
    }

    void test(Client& client, cmd_t cmd)
    {
        std::cout << "allo?\n";
        auto quest = std::get<std::string>(cmd.get_parameter("id"));

        std::cout <<  "Get user\n";

        auto user = cmd.command.get_issuing_user();
        if(cmd.command.get_guild().voice_members.count(user.id) < 1) {
            cmd.reply("Where voice channel?");
            return;
        }

        if(!((dpp::guild)cmd.command.get_guild()).connect_member_voice(cmd.command.usr.id)) {
            cmd.reply("Нахуя?");
            return;
        }

        std::cout << "Video shit\n";

        Music::YTVideo video;
        video.id = quest;

        dpp::voiceconn* connection = cmd.from->get_voice(cmd.command.guild_id);

        cmd.thinking();

        std::cout << "I try read\n";
        auto stream = video.read(*connection->voiceclient);
        std::cout << "I call video.read() - it's fine\n";
        
        auto& v = connection;
        if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
            cmd.edit_response("Ну, библиотека говна. Я в ауте.");
            return;
        }

        std::cout << "Всё норм\n";
        try {
            while(stream.read() == 0)
            {
                while(!connection || !connection->voiceclient || !connection->voiceclient->is_ready())
                {
                    std::cout << "я просто здесь, я просто да...\n";
                }
            }
            std::cout << "try send\n" << stream.read_code << "\n";
            cmd.edit_response(Music::YTVideo::en_Status_messages[stream.read_code]);
        } catch(const std::string& errMsg){

        }
    }
}
