#include "xlcomm/base/count_down_latch.h"

namespace xlcomm {
namespace base {

CountDownLatch::CountDownLatch(int count)
    : count_(count) {
}

void CountDownLatch::Wait() {
  std::unique_lock<std::mutex> lock(mutex_);
  while (count_ > 0) {
    condition_.wait(lock);  // 条件变量需要unique_lock类型，wait内部会调用解锁
  }
}

void CountDownLatch::CountDown() {
  std::lock_guard<std::mutex> lock(mutex_);
  --count_;
  if (count_ == 0) {
    condition_.notify_all();
  }
}

int CountDownLatch::count() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return count_;
}

}  // namespace base
}  // namespace xlcomm