#include <client/client.hpp>
#include <events/all.hpp>
#include <commands/load.hpp>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <client/player/Sources/YTVideo.hpp>
#include "client/Threading/ThreadManager.hpp"
#include <client/player/player.hpp>
#include <util/database.hpp>
#include <util/cfgparser.hpp>

std::string BOT_TOKEN;
std::string Music::YTVideo::ACCESS_TOKEN;

void load_token();

database_t database;

namespace statistics {
    auto timer_messages = std::chrono::system_clock::now();
    int timer_messages_count = 0;
    int timer_commands_count = 0;
}

void func_thread_timer();


int main(int argc, char** argv)
{

    //
    //@WARNING
    //
    // One fork work only
    //
    
    load_token();
    system("mkdir -p music && find music/ -size 0 -print -delete && rm music/*.part");
    srand(time(0));
    Client bot(BOT_TOKEN);
    database.connect("tcp://127.0.0.1:3306", "root", "12");


    for(int i = 1; i < argc; ++i)
    {
        if(argv[i] == std::string("--init-mysql")) {
            database._create_tables();
            return 0;
        }
        if(std::string("--help") == argv[i] || argv[i] == std::string("-h"))
        {
            std::cout << "\t--init-mysql - Initialize tables in database.\n";
            return 0;
        }
    }

    commands::Load(bot);

    //@events
    std::cout << "Accept logger\n";
    bot.on_log(dpp::utility::cout_logger());
    std::cout << "onready\n";
    bot.on_ready([&bot](const dpp::ready_t& d) { events::on_ready(bot, d); });
    bot.on_slashcommand([&bot](const dpp::slashcommand_t& d) {
        static Threading::ThreadPool pool(4);

        ++statistics::timer_commands_count;

        pool.add_func([d, &bot]() -> void {
            events::on_slashcommand(bot, d);
        });
    });

    bot.on_voice_ready(Music::Player::on_ready);
    bot.on_voice_track_marker(Music::Player::on_marker);
    bot.on_voice_state_update(Music::Player::on_voice_state_update);

    bot.on_message_create([&bot](const dpp::message_create_t event) {

        if (event.msg.content == ".join") {
            dpp::guild * g = dpp::find_guild(event.msg.guild_id);
            if (!g->connect_member_voice(event.msg.author.id)) {
                bot.message_create(dpp::message(event.msg.channel_id, "You don't seem to be on a voice channel! :("));
            }
        }

        ++statistics::timer_messages_count;

        if(event.msg.author.is_bot() || event.msg.author.is_bot_http_interactions()) return;

        database.update_ifnotexist_create({
               {
                   "messages", "messages + 1"
               }
           },
           profile {
               .id = event.msg.author.id
           },
           event.msg.author
        );



        if(event.msg.content == "?reloadcmd" && event.msg.author.id == dpp::snowflake(333999936821526528U))
        {
            bot.message_create(dpp::message(event.msg.channel_id, "ok").set_reference(event.msg.id));
            for(auto& cmd: bot.slashcommands)
            {
                std::cout << cmd.second.name << " - " << cmd.second.id << "\n";
                bot.global_command_edit(cmd.second);
            }
        }
    });

    //@thread start
    std::thread timer_thread(func_thread_timer);
    //@start
    std::cout << "start\n";
    bot.start(dpp::st_wait);

    return 0;
}

void load_token()
{
    if(!std::filesystem::exists("bot.cfg")) {
        std::cout << "not exist bot.cfg\n";
        exit(1);
    } else {
        auto map = cfg_parse("bot.cfg");
        BOT_TOKEN = map["BOT_TOKEN"];
        Music::YTVideo::ACCESS_TOKEN = map["YT_ACCESS_TOKEN"];
    }
}

void func_thread_timer()
{
    while(true)
    {
        statistics::timer_messages_count = 0;
        statistics::timer_commands_count = 0;
        statistics::timer_messages = std::chrono::system_clock::now();
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
}
