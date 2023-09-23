#include "load.hpp"

#include "avatar.hpp"
#include "8ball.hpp"
#include "botinfo.h"
#include "test.hpp"

#define CREATE_CMD(NAME, DESCRIPTION) bot.slashcommands.insert({#NAME, Command(#NAME, DESCRIPTION, 0)}); NAME##_##create(bot.slashcommands[#NAME]);
#define CREATE_CMD_SN(NAME, DESCRIPTION, NAME_COMMAND) bot.slashcommands.insert({NAME_COMMAND, Command(NAME_COMMAND, DESCRIPTION, 0)}); NAME##_##create(bot.slashcommands[NAME_COMMAND]);


namespace commands {
    void Load(Client& bot)
    {
        CREATE_CMD(avatar, "get avatar of defined user/self")
        CREATE_CMD_SN(_8ball, "Thinking about some question? Ask the ball!", "8ball")
        CREATE_CMD(botinfo, "get information about bot. ¯\_(ツ)_/¯")
        CREATE_CMD(test, "what i do?")
    }
}

#undef CREATE_CMD
