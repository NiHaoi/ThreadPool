#pragma once
#include "threadPool.h"
inline ThreadPool::ThreadPool(size_t threads) : _run(true) {
    for (size_t i = 0; i <= threads; i++)
        _workers.emplace_back(
            [this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lck(_queue_mutex);
                        this->_condition.wait(lck, [this] { return !this->_run || !this->_tasks.empty(); });
                        if (!this->_run && this->_tasks.empty()) {
                            return;
                        }
                        task = std::move(this->_tasks.front());
                        this->_tasks.pop();
                    }
                    task();
                }
            });
}
template<typename F>
inline auto ThreadPool::commit(F && task) -> std::future<std::invoke_result_t<F>> {
    using return_type = std::invoke_result_t<F>;
    auto packaged_task = std::make_shared<std::packaged_task<return_type()>>(
        std::forward<F>(task)
    );
    std::future<return_type> reValue = packaged_task->get_future();  //in order to get the return value
    {
        std::unique_lock<std::mutex> lck(_queue_mutex);
        if (!_run) {                                                //The thread pool has been shut down.
			throw std::runtime_error("ThreadPool is stopped, cannot commit new tasks.");
        }
        _tasks.emplace([packaged_task] {(*packaged_task)(); });
    }
    _condition.notify_one();
    return reValue;
}
inline ThreadPool::~ThreadPool() {
   _run = false;
   _condition.notify_all();
   for (std::thread& worker : _workers) {
       if (worker.joinable()) {
           worker.join();
       }
   }
}