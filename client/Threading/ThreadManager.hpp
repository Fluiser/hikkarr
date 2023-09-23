#pragma once
#include <thread>
#include <condition_variable>
#include <list>
#include <mutex>
#include <functional>
#include <future>
#include <vector>
#include <iostream>

namespace Threading {

    class ThreadPool {
        std::list<std::pair<char*, std::function<void()>>> queue;
        std::condition_variable var;
        std::mutex mtx;

        std::vector<std::jthread> threads;

        static void _threading_handle(ThreadPool*);
        bool sleep = true;

        bool active = true;

        template<class Fn, class... Args>
        struct proxy {
            std::packaged_task<Fn> function;
            std::tuple<Args...> args;
        };
    public:
        void create(size_t size)
        {
            sleep = true;
            active = false;
            var.notify_all();
            threads.resize(size);
            active = true;
            for(size_t i = 0; i < size; ++i)
                threads[i] = std::jthread(ThreadPool::_threading_handle, this);
        }

        ThreadPool(size_t size)
        {
            this->create(size);
        }

        template<class F, class... Args>
        auto add_task(std::packaged_task<F(Args...)>&& _task, Args&&... args)
        {
            std::lock_guard $__(mtx);

            proxy<F(Args...), Args...>* prx = new proxy<F(Args...), Args...>();

            prx->function.swap(_task);
            prx->args = std::make_tuple(std::forward<Args>(args)...);

            queue.emplace_back((char*)prx, [prx]{
                proxy<F(Args...), Args...>* ptr = prx;
                std::apply(ptr->function, ptr->args);
            
                delete prx;     
            });

            if(sleep) {
                var.notify_all();
            }

            return prx->function.get_future();
        }

        void add_func(std::function<void()>&& fn)
        {
            std::cout << "Function add\n";
            queue.emplace_back((char*)nullptr, fn);
            if(sleep) {
                var.notify_all();
            }
        }

        void stop()
        {
            active = false;
            var.notify_all();
            for(auto& x: threads)
                if(x.joinable())
                    x.join();
        }

        void drop()
        {
            if(active) {
                try {
                    for(auto& x: threads)
                        if(!x.joinable())
                            x.detach();
                } catch(...) {}
                active = false;
                var.notify_all();
            }
        }

        ~ThreadPool()
        {
            this->stop();
        }
    };

}
