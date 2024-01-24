#include "Source.hpp"
#include <oggz/oggz.h>
#include <algorithm>
#include <dpp/dpp.h>
#include <unistd.h>
#include <stdlib.h>

namespace Music {

    MediaSource::~MediaSource()
    {
        if(pipe->is_open())
            pipe->close();
    }

    MediaStream::~MediaStream()
    {
    }

    void MediaStream::_callback()
    {
        if(src.pipe->is_open())
            src.pipe->async_read_some(boost::asio::buffer(src.buffer), [&](const boost::system::error_code& ec, size_t count){
                this->vc.send_audio_raw((uint16_t*)src.buffer.data(), count);
                this->_callback();
            });
    }

    MediaStream::read_status MediaStream::read()
    {
        if(src.pipe == nullptr)
            return MediaStream::ERR;
        std::cout << "try download\n";
        src.download();
        _callback();


            
        return MediaStream::read_status::OK;
    }


    MediaStream MediaSource::read(dpp::discord_voice_client& client, boost::process::async_pipe* p)
    {
        if(buffer.capacity() < MUSIC__BUFFER_SIZE)
            buffer.resize(MUSIC__BUFFER_SIZE);
        pipe = p;
        return MediaStream(*this, client);
    }

}
