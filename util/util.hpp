#pragma once
#include <dpp/dpp.h>
#include <boost/stacktrace.hpp>

#define BOT_VERSION "0.0.1b"

template<class T, int n>
constexpr int array_size(T(&)[n]) noexcept
{
    return n;
}

template<class T>
void ZeroMem(T& obj)
{
    memset((void*)&obj, 0, sizeof(obj));
}

template<class... Args>
inline void stack_log(Args&&... args)
{
    auto t = {
        (std::cout << args, 0)...
    };

    (void)t;
    std::cout << '\n';
    std::cout << boost::stacktrace::stacktrace();
    std::cout << '\n';
}

template<typename T> std::string fast_tostring(const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}
