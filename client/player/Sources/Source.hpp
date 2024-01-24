#pragma once
#include <iterator>
#include <memory>
#include <fstream>
#include <util/util.hpp>
#include <stdint.h>
#include <boost/process/async_pipe.hpp>
#include <boost/process.hpp>
#include <boost/asio.hpp>


#undef ERROR

#define MUSIC__BUFFER_SIZE 11520

namespace Music {
    class MediaStream;
    
    class MediaSource {
    public:
        std::vector<char> buffer;
        std::string id;
        std::string path_to_file;
        boost::process::async_pipe* pipe;
        //SharedFile sfile;

        virtual int download()
        {
            std::cout << "What the fuck are you doing?";
            return {};
        }; // start download

        /**
         * MediaSource must be alive when exist MediaStream
        */

        ~MediaSource();

        MediaStream read(dpp::discord_voice_client&, boost::process::async_pipe*);

        virtual const char* formMessage(const std::string& lang, int code)
        {
            std::cout << "call virtual method\n";
            return "call virtual method";
        }
    };

    class MediaStream {
        friend MediaSource;
        MediaSource& src;
        dpp::discord_voice_client& vc;

        MediaStream(MediaSource& _source, dpp::discord_voice_client& _client): src(_source), vc(_client) { }
    public:

        int read_code = 0;

        ~MediaStream();

        enum read_status: int {
            OK = 0,
            ERR = 1,
            END = -1
        };


        read_status read();
        void _callback();
    };

}

// namespace Music {
//     class MediaStream;
//     class MediaSource {
//     public:
//         std::string id;
//         std::string path_to_file;

//         virtual std::string download()
//         {
//             std::cout << "What the fuck are you doing?";
//             return {};
//         }; // start download

//         MediaStream begin();
//         MediaStream end();
//     };

//     class MediaStream: public std::iterator<
//                             std::input_iterator_tag,
//                             MediaSource, // value_type
//                             MediaSource, // dif_type
//                             MediaSource*, // pointer
//                             std::pair<uint8_t*, size_t>> { // reference
//         OGGZ* ogg_file;

//         uint8_t buff[MUSIC__BUFFER_SIZE];
//         size_t size_buff = 0;

//         MediaSource* src = nullptr;

//     public:
//         bool isEnd = false;


//         explicit MediaStream(MediaSource& _source): src(&_source) {
//             std::cout << src << "\n" << src->path_to_file.size();
//         }
//         ~MediaStream() {}

//         bool operator==(const MediaStream& );
//         bool operator!=(const MediaStream& );

//         std::pair<uint8_t*, size_t> operator*();
//         bool isValid();

//         MediaStream& operator++();
//         MediaStream operator++(int);
//     private:
//         friend MediaSource;
//         int offset;
//     };

