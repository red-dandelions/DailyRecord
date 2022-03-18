#include "thread_pool.h"

namespace dailyrecord
{
  
ThreadPool::ThreadPool(size_t threads) : stop_(false)
{
  //获取当前可用硬件实现支持的并发线程数
  size_t n = std::thread::hardware_concurrency();
  n = std::min(n, threads);
  for (size_t i = 0; i < n; i++)
  {
    workers_.emplace_back([this]{ Loop(); });
  }
}

ThreadPool::~ThreadPool()
{
  {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    stop_ = true;
  }
  //通知所有线程完成任务后退出循环
  condition_.notify_all();
  for (std::thread &worker : workers_)
  {
    worker.join();
  }
}

void ThreadPool::Loop()
{
  while (true)
  {
    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      //如果stop_不为真，且任务队列还有没执行完的任务，继续执行
      condition_.wait(lock, [this] { return this->stop_ || !tasks_.empty(); });
      if (stop_ && tasks_.empty()) {
        return;
      }
      task = std::move(tasks_.front());
      tasks_.pop();
    }
    task();
  }
}

void ThreadPool::Run(const std::function<void()>& func)
{
  //增加一个任务，然后通知一个线程执行
  {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    tasks_.emplace(func);
  }
  condition_.notify_one();
}

void ThreadPool::AddThread()
{
  //增加一个线程
  std::unique_lock<std::mutex> lock(queue_mutex_);
  workers_.emplace_back([this]{ Loop(); });
}

} //namespace dailyrecord