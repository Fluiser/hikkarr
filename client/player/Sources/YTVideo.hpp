#pragma once
#include <string>
#include <dpp/httpsclient.h>
#include <util/type.h>
#include <future>
#include <thread>
#include <mutex>
#include "Source.hpp"

namespace {
    const char* const IndexAlphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-_";
    constexpr int IndexAlphabet_size = 38;

}

void load_token();

namespace Music {
    std::string id_to_string(size_t);

    class YTVideo : public MediaSource{
        friend void ::load_token();
    private:
        boost::process::child proc;
        static std::string ACCESS_TOKEN;
    public:
        inline static constexpr const char* en_Status_messages[] = {
                "Download",
                "Is stream",
                "Video is undefined",
                "Video/id is invalid",
                "Service not respond",
                "Video is long (>100MB)"
        };
        inline static constexpr const char* ru_Status_messages[] = {
                "Загрузка",
                "Это стрим",
                "Видео не передано",
                "Видео/id не валидно",
                "Сервис не ответил",
                "Видео слишком большое (>100MB)"
        };

        static std::vector<YTVideo> search(const std::string& query);

        std::string name;
        std::string thumbnail;
        i64 size; // in seconds. If it's stream = -1.
        //1 block = 20 min

        YTVideo() {}
        YTVideo(const YTVideo&) {}
        ~YTVideo();

        virtual int download() override;

        virtual const char* formMessage(const std::string& lang, int code) override
        {
            if(code >= array_size(en_Status_messages)) {
                code = 4;
            }

            if(lang == "ru") {
                return ru_Status_messages[code];
            } else {
                return en_Status_messages[code];
            }
        }
    private:
        static std::pair<int, std::string> runDownload(std::string id);
    };
}
