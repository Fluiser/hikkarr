#pragma once
#include <map>
#include <string>
#include <mutex>
#include <chrono>

class SharedFile {
    const std::string* id = nullptr;

    struct _data {
        struct piece_of_shit {};
        std::mutex mtx;
        int size_use;
        decltype(std::chrono::system_clock::now()) lastuse;

        _data(const _data&) {}
        _data(_data&&) {}
        _data(int) {}
    };

    inline static std::map<std::string, _data> locks = {};
public:
    SharedFile() = default;

    void update_last_use();

    void create(const std::string& id_);


    std::lock_guard<std::mutex> lock();

    ~SharedFile();
};
