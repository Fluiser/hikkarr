#include "8ball.hpp"
#include <client/embed.hpp>
#include <stdlib.h>
#include <util/util.hpp>

namespace {
    const std::string answers_en[]  =  {
        "**As I see it, yes**\n\n",
        "**It is certain**\n\n",
        "**It is decidedly so**\n\n",
        "**Most likely**\n\n",
        "**Outlook good**\n\n",
        "**Signs point to yes**\n\n",
        "**Without a doubt**\n\n",
        "**Yes**\n\n",
        "**Yes – definitely**\n\n",
        "**Reply hazy, try again**\n\n",
        "**Ask again later**\n\n",
        "**Better not tell you now**\n\n",
        "**Cannot predict now**\n\n",
        "**Concentrate and ask again**\n\n",
        "**Don't count on it**\n\n",
        "**My reply is no**\n\n",
        "**My sources say no**\n\n",
        "**Outlook not so good**\n\n",
        "**Very doubtful**\n\n"
    };
    const std::string answers_ru[] = {
        "**Как я вижу, да**\n\n",
        "**Это точно**\n\n",
        "**Это явно так**\n\n",
        "**Скорее всего**\n\n",
        "**Перспективы хорошие**\n\n",
        "**Знаки указывают, что да**\n\n",
        "**Без сомнения**\n\n",
        "**Да**\n\n",
        "**Да, безусловно**\n\n",
        "**Ответ туманен, попробуй еще раз**\n\n",
        "**Повтори попытку позже**\n\n",
        "**Сейчас лучше не рассказывать**\n\n",
        "**Невозможно предсказать сейчас**\n\n",
        "**Сконцентрируйся и спроси еще раз**\n\n",
        "**Не рассчитывай на это**\n\n",
        "**Мой ответ - нет**\n\n",
        "**Мои источники говорят, что нет**\n\n",
        "**Перспектива не так хороша**\n\n",
        "**Очень сомнительно**\n\n"
    };
}

namespace commands {
    void _8ball_create(Command& cmd)
    {
        cmd.add_localization("ru", "шар", "Получить предсказание на свой вопрос");
        cmd.add_option(
            dpp::command_option(dpp::co_string, "question", "question", true)
            .add_localization("ru", "question", "вопрос")
            .set_max_length(1950)
        );
        cmd.run = _8ball;
    }

    void _8ball(Client& client, cmd_t cmd)
    {
        auto quest = std::get<std::string>(cmd.get_parameter("question"));

        Embed embed;
        embed.set_description( (cmd.command.locale == "ru" ?
                        answers_ru[(rand() % array_size(answers_ru))] :
                        answers_en[(rand() % array_size(answers_en))])
                            + quest
                        );

        cmd.reply(dpp::message(cmd.command.channel_id, embed));
    }
}