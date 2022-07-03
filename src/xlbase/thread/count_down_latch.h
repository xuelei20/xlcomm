// 线程同步原语高级封装——倒数器。用于一个或多个线程等待一个或多个任务完成
// Author: xuel

#ifndef XLBASE_COUNTDOWNLATCH_H_
#define XLBASE_COUNTDOWNLATCH_H_

#include <memory>
#include <mutex>

#include "xlcomm_define.h"

namespace xlbase {

class XLCOMM_API CountDownLatch {
public:
  explicit CountDownLatch(int count);
  XLCOMM_NOT_COPY_OR_MOVE(CountDownLatch);

  void Wait();
  void CountDown();
  int count() const;

private:
  struct Impl {
    mutable std::mutex mutex;
    std::condition_variable condition;
    int count = 0;
  };
  std::unique_ptr<Impl> impl_;
};

}  // namespace xlbase

#endif  // XLBASE_COUNTDOWNLATCH_H_