#include "YTVideo.hpp"
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>
#include <json.hpp>
#include <format>
#include <sstream>
#include <iomanip>
#include <string>
#include <util/util.hpp>
#include <filesystem>
#include <memory>
#include <boost/process.hpp>

using json = nlohmann::json;

namespace Music {

std::string id_to_string(size_t x)
{
    if(x == 0) return std::string{IndexAlphabet[0]};
    std::string str;
    while(x != 0)
    {
        str.push_back(x%IndexAlphabet_size);
        x /= IndexAlphabet_size;
    }

    return str;
}

std::string url_encode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

using namespace curlpp;

    std::vector<YTVideo> YTVideo::search(const std::string& query)
    {
        curlpp::Cleanup oldshit;
        curlpp::Easy req;

        std::ostringstream os;

        req.setOpt(options::Url(
                std::format("https://www.googleapis.com/youtube/v3/search"
                            "?part=snippet"
                            "&maxResults=5"
                            "&q={}"
                            "&key={}",
                        url_encode(query),
                        YTVideo::ACCESS_TOKEN)
            ));
        req.setOpt(options::WriteStream(&os));
        try {
            req.perform();
        } catch(const LibcurlRuntimeError& err) {
            std::cout << "[" << err.whatCode() << "] " << err.what() << "\n";
            return {};
        }

        auto data = json::parse(os.str());
        auto items = data["items"];

        std::cout << os.str() << "\n";

        std::vector<YTVideo> handledItems;
        handledItems.resize(5);


        {
            int idx = 0;
            for(const auto& item: items)
            {
                auto& video = handledItems[idx++];
                auto snippet = item["snippet"];
                video.id = item["id"]["videoId"];
                video.name = snippet["title"];
                video.size = (snippet["liveBroadcastContent"] != "none" ? -1 : 0);

                if(item.count("thumbnails") > 0) {
                    auto tromb = item["thumbnails"];
                    if(tromb.count("high") > 0) {
                        video.thumbnail = tromb["high"]["url"];
                        continue;
                    }
                    if(tromb.count("medium") > 0) {
                        video.thumbnail = tromb["medium"]["url"];
                        continue;
                    }
                    if(tromb.count("default") > 0) {
                        video.thumbnail = tromb["default"]["url"];
                        continue;
                    }
                }
            }
        }


        return handledItems;
    }

    int YTVideo::download()
    {

        std::cout << "call fppen:\npipe is " << (pipe != nullptr);
        proc = boost::process::child(
                std::format("yt-dlp "
                            "-q "
                            // "--sponsorblock-remove ALL "
                            "-f 251 "
                            "--http-chunk-size 2M "
                            "-x --audio-format opus "
                            "--audio-quality 0 "
                            "-o - "
                            "\"{}\""
                        // " | ffmpeg -acodec opus -i - -f s16le -ar 48000 -ac 2 -"
                        , this->id),
                boost::process::std_in < (*pipe));



    }

    YTVideo::~YTVideo()
    {
    }

    std::pair<int, std::string>
    YTVideo::runDownload(std::string id)
    {
        std::cout << "Hello there:: YtVideo \n";
        if(std::filesystem::exists(std::format("music/{}.opus", id))) {
            return {0, std::format("music/{}.opus", id)};
        }

        std::condition_variable cv;
        std::mutex mtx;

        std::packaged_task<int()> task_download([id, &cv, &mtx]{
            std::string command = std::format("../API/ytvideo.py \"{}\""
                                              //  " --max-filesize 100M" handle in python file
                                              " --sponsorblock-remove ALL"
                                              " -f bestaudio" // лучшее аудио
                                              " --http-chunk-size 2M" // чтобы постоянно не спамить ffmpeg'ом, если это лайв стрим. Интервал в 2мб, короче.
                                              " -x --audio-format opus" // только аудио, формат opus
                                              " --audio-quality 0" // самое крутое.
                                              " -o \"music/%(id)s\"", id);
            int code;
            try {
                code = boost::process::system(command);
            } catch(const boost::process::process_error& err) {
                code = 0;
                std::cout << "буст сходил нахуй " << err.what() << "\n";
            }
            if(code < 0 || code > array_size(YTVideo::en_Status_messages)) {
                std::cout << "YTVideo::runDownload::cmd return code: " << code << "\n";
                code = 4;
            }
            return code;
        });

        auto future = std::make_shared<std::future<int>>(task_download.get_future());
        std::thread threadDownload(std::move(task_download));


        future->wait_for(std::chrono::seconds(60));

        if(future->valid()) {
            threadDownload.join();

            int pc = future->get();
            if(pc > 1) {
                return {pc, ""};
            }

            if(pc == 1) {
                return {1, std::format("music/{}.part", id)};
            }
            if(pc == 0) {
                return {0, std::format("music/{}.opus", id)};
            }
        }

        std::thread th([future]{
            future->wait();
        });
        th.detach();
        threadDownload.detach();

        return {4, ""}; // just error;
    }
}
