ThreadPool
==========

Nothing special to see here, just a practice project.

Basic usage:
```c++
// create thread pool
ThreadPool pool(5);

auto result = pool.commit([parameter]{return return_value});

result.wait();

std::cout << result.get() << std::endl;

```
