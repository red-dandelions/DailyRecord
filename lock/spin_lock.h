#ifndef __SPIN_LOCK_H__
#define __SPIN_LOCK_H__

#include <atomic>

namespace dailyrecord
{
  class SpinLock
  {
  public:
    SpinLock() : atomic_bool_(false) { }

    void lock() {
      bool cmp = false;
      while (!atomic_bool_.compare_exchange_weak(cmp, true)) {
        // atomic_bool_与cmp比较，如果相等，则atomic_bool_的值被true替换
        // 如果不相等，则cmp的值被atomic_bool_的值替换，所以需要重新给cmp赋值
        cmp = false;
      }
    }


    void unlock() {
      atomic_bool_.store(false);
    }

  private:
   std::atomic_bool atomic_bool_;
  };
} //namespace dailyrecord

#endif __SPIN_LOCK_H__