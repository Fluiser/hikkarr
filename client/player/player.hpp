#pragma once

#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include "Sources/Source.hpp"
#include <future>
#include <util/type.h>
#include <client/client.hpp>
#include <memory>

/*
 * ENDSONG - marker in song.
 * */

namespace Music {

    class Player
    {
        enum class play_mode_t: int{
            repeat_song = 1, // repeat song
            repeat_queue = 2, // save queue
            exit_from_voice = 3 // delete queue
        } loop_mode = play_mode_t::repeat_queue;

        enum class player_status_t: int {
            playing = 0,
            empty = 1,
            wait = 2,
            stop = 3
        } status = player_status_t::empty;

        u64 idx_song = 0;

        std::list<MediaSource> queue;

        //guild_id, Player
        inline static std::map<dpp::snowflake, std::unique_ptr<Player>> players{};

        dpp::snowflake guild_id;
        dpp::snowflake channel_id;

        Client& client;

        dpp::voiceconn* vc;

        std::string lang;
        dpp::snowflake _textchannel;
    public:

        Player(Client& client);

        static Player* create(Client&, dpp::snowflake guild_id, dpp::snowflake user_id);

        void stream();
        void handle_playermode();
        void handle_mediaresultstream(int code);

        void set_msgcall(dpp::snowflake textchannel_id, std::string l);

        void add_source(std::string src);
        void add_source(MediaSource src);



        static void on_ready(const dpp::voice_ready_t&);
        static void on_marker(const dpp::voice_track_marker_t&);
        static void on_voice_state_update(const dpp::voice_state_update_t&);
        static Player* get_player(dpp::snowflake guild_id);
    };

}
