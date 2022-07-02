// 线程同步原语高级封装——倒数器。用于一个或多个线程等待一个或多个任务完成
// Author: xuel

#ifndef XLBASE_COUNTDOWNLATCH_H_
#define XLBASE_COUNTDOWNLATCH_H_

#include <condition_variable>
#include <mutex>

#include "xlcomm_define.h"

namespace xlbase {

class XLCOMM_API CountDownLatch {
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