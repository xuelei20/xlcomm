// Author:  xuel
// Purpose: 线程同步原语高级封装——倒数器

#ifndef XLBASE_COUNTDOWNLATCH_H_
#define XLBASE_COUNTDOWNLATCH_H_

#include <condition_variable>
#include <mutex>

namespace xlbase {

class CountDownLatch {
public:
  explicit CountDownLatch(int count);
  void Wait();
  void CountDown();
  int count() const;

private:
  mutable std::mutex mutex_;
  std::condition_variable condition_;
  int count_;
};

}  // namespace xlbase

#endif  // XLBASE_COUNTDOWNLATCH_H_