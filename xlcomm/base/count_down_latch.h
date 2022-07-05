// 线程同步原语高级封装——倒数器。用于一个或多个线程等待一个或多个任务完成
// Author: xuel

#ifndef XLCOMM_BASE_COUNTDOWNLATCH_H_
#define XLCOMM_BASE_COUNTDOWNLATCH_H_

#include <memory>
#include <mutex>
#include <condition_variable>

#include "xlcomm/xlcomm_define.h"

namespace xlcomm {
namespace base {

class XLCOMM_API CountDownLatch {
 public:
  explicit CountDownLatch(int count);
  CountDownLatch(const CountDownLatch&) = delete;
  CountDownLatch& operator=(const CountDownLatch&) = delete;

  void Wait();
  void CountDown();
  int count() const;

 private:
  mutable std::mutex mutex_;
  std::condition_variable condition_;
  int count_ = 0;
};

}  // namespace base
}  // namespace xlcomm

#endif  // XLCOMM_BASE_COUNTDOWNLATCH_H_