#include <iostream>
#include <mutex>
#include <thread>

std::mutex mutex_;

int g_val = 0;

int main()
{
  std::thread _th1([]{
    std::lock_guard<std::mutex> lock(mutex_);
    for (int i = 0; i < 100000; ++i) {
      ++g_val;
    }
  });
  std::thread _th2([]{
    std::lock_guard<std::mutex> lock(mutex_);
    for (int i = 0; i < 100000; ++i) {
      ++g_val;
    }
  });
  _th1.join();
  _th2.join();
  std::cout << "g_val: " << g_val << std::endl;
  return 0;
}