#include "botinfo.h"
#include <util/util.hpp>
#include <client/embed.hpp>
#include <util/SysAPI.h>
#include <format>

namespace statistics {
    extern int timer_messages_count;
    extern int timer_commands_count;
}

std::string cnvr(size_t count) {
    const char* cvrt[] = {"bytes", "kb", "mb", "gb", "tb", "pb"};
    int idx = 0;
    float size = count;

    while (size >= 1024 && idx < array_size(cvrt)) {
        size /= 1024;
        idx++;
    }


    return std::format("{:.2f}{}", size, cvrt[idx]);
}

namespace commands {
    void botinfo(Client& client, cmd_t inter)
    {
        SysAPI::sys_info sysinfo;
        SysAPI::get_sys_info(sysinfo);

        auto embed = Embed();

        std::string strUptime = "";
        {
            auto uptime = client.uptime();
            int count = 0;
            if(uptime.days > 1) {
                strUptime += std::to_string(uptime.days);
                strUptime += "d";
                ++count;
            }
            if(uptime.hours > 1) {
                strUptime += std::to_string(uptime.hours);
                strUptime += "h";
                ++count;
            }
            if(uptime.mins > 1) {
                strUptime += std::to_string(uptime.mins);
                strUptime += "m ";
                ++count;
            }
            if(uptime.secs > 1 && count < 3) {
                strUptime += std::to_string(uptime.secs);
                strUptime += "s ";
            }
        }
        if(!strUptime.empty())
            strUptime.erase(strUptime.size() - 1);
        embed.description = std::vformat(
            (inter.command.locale == "ru" ?
                "**Осколки**: {}"
                "\n**ОЗУ**: [`{}`/`{}`] | **Исп.**: `{}`"
                "\n**ЦП**: `{}%`"
                "\n**Время жизни осколка**: `{}`"
                "\n\n**[Сервер тех. поддержки и место, где жили жабы](http://discord.gg/ErBU7UG).**"
                "\nОбработка: `{} сбщ/мин | {} кмд/мин`"
                "\n\n**Серверов:** `{}`"
                "\n**Кэшировано пользователей**: `{}`"
                //"\n**Уник. пользователей**: `{}`"
                "\n**Каналов**: `{}`\n\n"
                "Версия: `{}`\nНаписан в одиночку уставшим чуваком.\nИспользуемые инструменты: {}\nПиво 2л":

                "**Shards**: {}"
                "\n**RAM**: [`{}`/`{}`] | **Usg.**: `{}`"
                "\n**CPU**: `{}%`"
                "\n**Uptime**: `{}`"
                "\n\n**[Tech. support and place where do quacks frogs](http://discord.gg/ErBU7UG).**"
                "\nHandling: `{} msgs/min | {} cmds/min`"
                "\n\n **Guilds**: `{}`"
                "\n **Cached users**: `{}`"
                //"\n **Uniq. users**: `{}`"
                "\n **Channels**: `{}`"
                "\n\nVersion: `{}`\nWritten by a tired dude.\nLTools used: {}\nBeer 2l" ),
            std::make_format_args(client.get_shards().size(),
                                  cnvr(sysinfo.totalmem-sysinfo.freemem),
                                  cnvr(sysinfo.totalmem),
                                  cnvr(sysinfo.usagemem),
                                  sysinfo.cpu_usage,
                                  strUptime,
                                  statistics::timer_messages_count,
                                  statistics::timer_commands_count,
                                  dpp::get_guild_count(),
                                  dpp::get_user_count(),
                                  dpp::get_channel_count(),
                                  BOT_VERSION,
                                  "CMake 3.26\n"
                                  "Clang / MVSC, C++20\n"
                                  "Boost 1.80 [Asio, process, etc]\n"
                                  "OpenSSL 3.1.1\n"
                                  "ZLIB 1.2.13\n"
                                  "libsodium idk_ver")
            );
        inter.reply(dpp::message(inter.command.channel_id, embed));
    }
    void botinfo_create(Command& cmd)
    {
        cmd.add_localization("ru", "botinfo", "Информация про это ведро. ¯\_(ツ)_/¯");
        cmd.run = botinfo;
    }

}
