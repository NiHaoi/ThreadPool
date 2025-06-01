#include <iostream>
#include "threadPool.hpp"
void printHello() {
	std::cout << "Hello, World!" << std::endl;
}
int add(int a, int b) {
	return a + b;
}
int main(void) {
	ThreadPool pool(5);
	pool.commit(printHello);
	auto addResult = pool.commit(std::bind(add, 1, 2));
	addResult.wait();
	std::cout << addResult.get() << std::endl;
	std::vector<std::future<int>> results;
	for (int i = 1; i <= 40; i++) {
		results.emplace_back(pool.commit(
			[i]{
				int sum = 0;
				while (sum < 200) {
					sum += i;
				}
				return sum;
			}
		));
	}

	for (auto &&i:results) {
		i.wait();
		std::cout << i.get() << std::endl;
	}
	return 0;
}
