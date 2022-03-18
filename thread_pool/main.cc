#include "thread_pool.h"
#include <iostream>

using namespace dailyrecord;

void func() {
  std::cout << "test!" << std::endl;
}


int main() {

  ThreadPool tp(4);

  tp.Run(func);

  tp.Run([]{ std::cout << "test second!" << std::endl; });
  
  return 0;
}
