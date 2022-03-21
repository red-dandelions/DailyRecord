#ifndef __BLK_QUEUE_H__
#define __BLK_QUEUE_H__

#include <condition_variable>
#include <mutex>
#include <deque>
#include <chrono>

namespace DailyRecord
{

  template <typename _Task>
  class BlockingQueue
  {
  public:
    BlockingQueue(size_t cap = 128) : cap_(cap) {}  //默认容量为128
    virtual ~BlockingQueue() {}

    void Enqueue(const _Task &task)
    {
      std::unique_lock<std::mutex> lock(mutex_);
      while (task_.size() >= cap_)  //如果队列size超过cap，则等待出队之后再入队
      {
        cv_full_.wait(lock);
      }
      task_.push_back(task);
      // 入队之后通知可以出队
      cv_empty_.notify_all();
    }

    bool TryEnqueue(const _Task &task, uint32_t wait)
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (task_.size() >= cap_)
      {
        cv_full_.wait_for(lock, std::chrono::milliseconds(wait)); //等待wait毫秒，如果wait毫秒之后还是超过容量，则返回false
      }
      if (task_.size() >= cap_)
      {
        return false;
      }
      task_.push_back(task);
      // 入队之后通知可以出队
      cv_empty_.notify_all();
    }

    _Task Dequeue()
    {
      std::unique_lock<std::mutex> lock(mutex_);
      while (task_.empty())
      {
        //等待队列不为空
        cv_empty_.wait(lock);
      }
      _Task task = task_.front();
      task_.pop_front();
      // 出队后通知可以入队
      cv_full_.notify_all();
      return task;
    }

    bool TryDequeue(_Task *task, uint32_t wait)
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (task_.empty())
      {
        // 尝试wait毫秒之后出队，如果为空则返回false
        cv_full_.wait_for(lock, std::chrono::milliseconds(wait));
      }
      if (task_.empty())
      {
        return false;
      }
      *task = task_.front();
      task_.pop_front();
      // 出队之后通知可以入队
      cv_full_.notify_all();
      return true;
    }

    // return size
    size_t Size()
    {
      std::unique_lock<std::mutex> lock(mutex_);
      return task_.size();
    }

    bool Empty()
    {
      std::unique_lock<std::mutex> lock(mutex_);
      return task_.size() == 0;
    }

    bool Full(uint32_t wait = 1)
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (task_.size() < cap_)
      {
        return false;
      }
      cv_full_.wait_for(lock, std::chrono::milliseconds(wait));
      return task_.size() >= cap_;
    }

    void Clear(void (*deleteFunc)(_Task) = nullptr)
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (deleteFunc != nullptr)
      {
        while (!task_.empty())
        {
          _Task task = task_.front();
          deleteFunc(task);
          task_.pop_front();
        }
      }
      else
      {
        task_.clear();
      }
    }

  private:
    size_t cap_;                       //队列大小
    std::mutex mutex_;                 //互斥量
    std::condition_variable cv_full_;  // full条件变量
    std::condition_variable cv_empty_; // empty条件变量
    std::deque<_Task> task_;           //使用双端队列存Task

  }; // class BlockingQueue

} // namespace DailyRecord

#endif // __BLK_QUEUE_H__