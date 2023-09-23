#pragma once
#include <dpp/dpp.h>

class Embed: public dpp::embed {
public:
    Embed(): dpp::embed() {
        dpp::embed::set_color(rand() & 0xffffff);
    }
};
