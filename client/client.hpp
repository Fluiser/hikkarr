#pragma once
#include <dpp/dpp.h>
#include <map>
#include "command.hpp"

class Client: public dpp::cluster {

public:
    Client(     const std::string& token,
                uint32_t intents = dpp::intents::i_default_intents | dpp::intents::i_message_content,
                uint32_t shards = 0,
                uint32_t cluster_id = 0,
                uint32_t maxclusters = 1,
                bool compressed = true,
                dpp::cache_policy_t policy = { dpp::cache_policy_setting_t::cp_aggressive, dpp::cache_policy_setting_t::cp_aggressive, dpp::cache_policy_setting_t::cp_aggressive },
                uint32_t request_threads = 12,
                uint32_t request_threads_raw = 8) :
    dpp::cluster(token, intents, shards, cluster_id, maxclusters, compressed, policy, request_threads, request_threads_raw) {}

    std::map<std::string, Command> slashcommands;

};

typedef const dpp::slashcommand_t& cmd_t;
