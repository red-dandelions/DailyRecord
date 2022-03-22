#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>

std::mutex mutex_;
std::condition_variable cv_;

void thread_a()
{
  std::unique_lock<std::mutex> lock(mutex_);
  cv_.wait(lock);
  std::cout << "this is thread_a" << std::endl;
}

void thread_b()
{
  std::cout << "this is thread_b and will sleep 2 s" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));
  cv_.notify_all();
}

int main()
{
  std::thread _th1(thread_a);
  std::thread _th2(thread_b);

  _th1.join();
  _th2.join();

  return 0;
}