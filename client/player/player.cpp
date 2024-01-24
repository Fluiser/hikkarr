#include "player.hpp"

namespace Music {

    Player::Player(Client& _client): client(_client) {}

    Player* Player::create(Client& client, dpp::snowflake _guild_id, dpp::snowflake user_id)
    {

        dpp::guild* g = dpp::find_guild(_guild_id);

        if (!g->connect_member_voice(user_id)) {
            return nullptr;
        }

        auto* vc = client.get_shards().end()->second->get_voice(_guild_id);

        Player::players.emplace(_guild_id, std::make_unique<Player>(client));
        auto& player = Player::players.at(vc->channel_id);

        player->vc = vc;
        player->channel_id = vc->channel_id;
        player->guild_id = _guild_id;
        player->status = player_status_t::wait;


        return (player.get());
    }

    void Player::stream()
    {
        if(this->queue.empty()) return;
        if(this->status == player_status_t::stop) return;

        auto it = queue.begin();
        for(size_t idx = 0; idx <= this->idx_song
                            && it != queue.end(); ++idx, ++it);

        if(!this->vc->voiceclient || this->vc->voiceclient->is_connected()) {
            std::cout << "[!] Voice client is nullptr or not connected: " << this->guild_id << ": " << this->channel_id << "\n";
            return;
        }
        std::cout << "STREAM PLAYER FUCK UP ON ASYNC SERVICE.\n";
        return;
        auto stream = it->read(*this->vc->voiceclient, nullptr);

        int code;
        while(0 != (code = stream.read()))
        {
            // pause status;
        }
        if(!code) {
            this->status = player_status_t::playing;
        } else {
            this->handle_mediaresultstream(code);
            //erase bad song
            //send callback
            // try next song
        }

        this->handle_playermode();
    }

    void Player::on_ready(const dpp::voice_ready_t& data)
    {
        std::cout << data.voice_client << " is ready\n";
        auto* player = Player::get_player(data.voice_client->server_id);
        if(!player) {
            std::cout << "[!] on_ready event on not exist player: " << data.voice_channel_id << "\n";
            std::cout << "server_id: " << data.voice_client->server_id << ";\nIn map:";
            for(const auto& p: Player::players)
            {
                std::cout << p.first << "\n";
            }
            return;
        }
        if(player->status != player_status_t::wait)
            player->stream();
    }

    void Player::on_marker(const dpp::voice_track_marker_t& m)
    {
        if(m.track_meta == "endS") {
            auto* player = Player::get_player(m.voice_client->server_id);
            if(player) {
                player->stream();
            } else {
                std::cout << "[!] Get marker, but player not exist: channel<" << m.voice_client->channel_id << ">\n";
            }
        }
        std::cout << "GET marker: " << m.track_meta
        << "; from " << m.from << "\n";
    }

    void Player::on_voice_state_update(const dpp::voice_state_update_t& state)
    {
        auto* player = Player::get_player(state.state.guild_id);
        if(!player) return;

        if(state.state.channel_id != player->channel_id && state.state.user_id == player->client.me.id) {
            if(state.state.channel_id)
                player->channel_id = state.state.channel_id;
            else
                Player::players.erase(state.state.guild_id);
        }
    }

    Player* Player::get_player(dpp::snowflake gid)
    {
        if(Player::players.count(gid)) {
            return Player::players.at(gid).get();
        }
        return nullptr;
    }

    void Player::handle_mediaresultstream(int code)
    {
        if(this->_textchannel) {
            try {
                auto it = queue.begin();
                for(size_t i = 0; i <= idx_song && it != queue.end(); ++i, ++it);

                if(it != queue.end())
                    client.message_create_sync(dpp::message(_textchannel, it->formMessage(this->lang, code)));
            } catch(...) {}
        }
    }

    void Player::set_msgcall(dpp::snowflake textchannel_id, std::string l)
    {
        this->_textchannel = textchannel_id;
        this->lang = l;
    }

    void Player::handle_playermode()
    {
        switch(this->loop_mode)
        {
            case play_mode_t::repeat_song: break;
            case play_mode_t::repeat_queue: if(++idx_song >= queue.size()) idx_song = 0; break;
            case play_mode_t::exit_from_voice: if(++idx_song >= queue.size()) this->vc->disconnect();
        }
    }

}
