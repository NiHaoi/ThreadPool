#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <functional>
#include <future>
#include <type_traits>
class ThreadPool {
public:
	ThreadPool(size_t); // Constructor to initialize the thread pool
	~ThreadPool();
	template<typename F>
	auto commit(F&& task)
		-> std::future<std::invoke_result_t<F>>; // Method to commit the tasks to be executed by the threads
private:
	bool _run;
	std::queue<std::function<void()>> _tasks;
	std::vector<std::thread> _workers;
	std::mutex _queue_mutex; // Mutex for synchronizing access to the task queue
	std::condition_variable _condition;// Condition variable for notifying threads
};