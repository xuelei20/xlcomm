#include "xlcomm/base/count_down_latch.h"

namespace xlcomm {
namespace base {

CountDownLatch::CountDownLatch(int count)
  : impl_(std::make_unique<Impl>()) {
  impl_->count = count;
}

void CountDownLatch::Wait() {
  std::unique_lock<std::mutex> lock(impl_->mutex);
  while (impl_->count > 0) {
    impl_->condition.wait(lock);  // 条件变量需要unique_lock类型，wait内部会调用解锁
  }
}

void CountDownLatch::CountDown() {
  std::lock_guard<std::mutex> lock(impl_->mutex);
  --impl_->count;
  if (impl_->count == 0) {
    impl_->condition.notify_all();
  }
}

int CountDownLatch::count() const {
  std::lock_guard<std::mutex> lock(impl_->mutex);
  return impl_->count;
}

}  // namespace base
}  // namespace xlcomm