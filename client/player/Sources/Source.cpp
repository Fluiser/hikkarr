#include "Source.hpp"
#include <oggz/oggz.h>
#include <algorithm>
#include <dpp/dpp.h>

namespace Music {

    MediaStream::~MediaStream()
    {
        if(nullptr != file) {
            oggz_close(file);
        }
    }



    MediaStream::read_status MediaStream::read()
    {
        if(nullptr == file) {
            if(src.path_to_file.empty()){
                read_code = src.download().first;
                if(src.path_to_file.empty()) {
                    std::cout << "ERR code: " << read_code << "\n";
                    return MediaStream::read_status::ERR;
                }
            }

            FILE* hFile = fopen(src.path_to_file.c_str(), "r");

            std::cout << "Path: " << src.path_to_file << "\n";
            
            std::cout << (hFile) << "\n";

            file = oggz_open_stdio(hFile, OGGZ_READ);
            if(nullptr == file || NULL == file || src.path_to_file.empty()) {
                return MediaStream::read_status::ERR;
            }
            std::cout << "callback\n";    
            oggz_set_read_callback(file, -1, [](OGGZ* f, oggz_packet* pack, long s, void* userdata) -> int {
                auto& stream = *((MediaStream*)userdata);
                std::cout << "Try send\n";
                std::cout << "vc:" << stream.vc.is_ready() << ";\n";
                stream.vc.send_audio_opus(pack->op.packet, pack->op.bytes);
                std::cout << "send ahuenno\n";

                return 0;
            }, (void*)this);
        }
        std::cout << "fdsaf\n";

        const int rdBytes = oggz_read(file, MUSIC__BUFFER_SIZE);
        std::cout << "--read\n";
        if(0 == rdBytes) {
            this->vc.insert_marker("endS");

            if(nullptr != file) {
                oggz_close(file);
                file = nullptr;
            }

            return MediaStream::read_status::END;
        }

        return MediaStream::read_status::OK;
    }


    MediaStream MediaSource::read(dpp::discord_voice_client& client)
    {
        return MediaStream(*this, client);
    }

}
