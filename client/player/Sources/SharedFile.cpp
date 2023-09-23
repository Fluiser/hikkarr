#include "SharedFile.hpp"
#include <format>
#include <thread>
#include <chrono>
#include <filesystem>

void SharedFile::create(const std::string &id_)
{
    id = &id_;
    SharedFile::locks.emplace(id_, 0);
    SharedFile::locks.at(id_).lastuse = std::chrono::system_clock::now();
}

void SharedFile::update_last_use()
{
    if(locks.count(*id) > 0) {
        locks.at(*id).lastuse = std::chrono::system_clock::now();
    }
}

std::lock_guard<std::mutex> SharedFile::lock()
{
    return std::lock_guard(SharedFile::locks.at(*id).mtx);
}

SharedFile::~SharedFile()
{
    if(nullptr != id)
    if(!(*id).empty()) {
        auto& file = SharedFile::locks.at(*id);
        if((--(file).size_use) <= 0) {

            if(
                    std::chrono::duration_cast<std::chrono::seconds>(
                            (std::chrono::system_clock::now() - file.lastuse)
                    ).count() >= 14400 // 4h
            ) {
                SharedFile::locks.erase(*id);
                system(std::format("rm music/{}*", (*id)).c_str());
            } else {
                std::thread timeout([id = *id, secondWait = std::chrono::duration_cast<std::chrono::seconds>((std::chrono::system_clock::now() - file.lastuse)).count()]{
                    std::this_thread::sleep_for(std::chrono::seconds (14400 - secondWait));
                    SharedFile::locks.erase(id);
                    system(std::format("rm music/{}*", (id)).c_str());
                });

                timeout.detach();
            }

        }
    }
}
