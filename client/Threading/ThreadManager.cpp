#include "ThreadManager.hpp"

namespace Threading {

    void ThreadPool::_threading_handle(ThreadPool* p)
    {
        while(p->active) {
            std::unique_lock $__(p->mtx);
            p->var.wait($__);
            if(!p->queue.empty()) {
                auto fn = (p->queue.front()).second;
                p->queue.pop_front();
                $__.unlock();
                fn();
            } else {
                p->sleep = true;
            }
        }
    }

}
