#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace dailyrecord
{

class ThreadPool
{
private:
  std::vector<std::thread> workers_;        //工作线程
  std::queue<std::function<void()>> tasks_; //任务队列

  std::mutex queue_mutex_;                  //队列互斥器
  std::condition_variable condition_;       //条件变量

  bool stop_;                               //是否停止

  void Loop();                              //线程一直循环等待任务。

public:
  ThreadPool(size_t threads=1);             //最少一个线程
  ~ThreadPool();
  void Run(const std::function<void()> &func);  //启动一个任务
  void AddThread();                             //增加一个线程
};

} //namespace dailyrecord

#endif //__THREAD_POOL_H__