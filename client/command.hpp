#pragma once
#include <functional>
#include <string>
#include <dpp/dpp.h>

class Client;

class Command: public dpp::slashcommand {
public:
    Command(const std::string& name, const std::string& description, const dpp::snowflake& snowflake):
        dpp::slashcommand(name, description, snowflake) {}
    Command(const Command& cmd): dpp::slashcommand(cmd) {}
    Command() {}

    void (*run)(Client&, const dpp::slashcommand_t&);
    void (*autocomplete)(Client&, const dpp::autocomplete_t&);
};