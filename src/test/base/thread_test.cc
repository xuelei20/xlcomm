// 线程单元测试
// Author: xuel

#include "xlbase/thread/i_thread.h"

#include <assert.h>

#include <thread>
#include <iostream>

void TestCountDownLatch() {
  xlbase::CountDownLatch latch(3);
  std::cout << sizeof(latch) << std::endl;

  std::thread thread_master([&latch]() {
    latch.Wait();
    assert(latch.count() == 1);
  });

  std::thread thread_slave1([&latch]() {
    latch.CountDown();
  });

  std::thread thread_slave2([&latch]() {
    latch.CountDown();
  });

  std::thread thread_slave3([&latch]() {
    latch.CountDown();
  });

  thread_master.join();
  thread_slave1.join();
  thread_slave2.join();
  thread_slave3.join();
}

int main() {
  TestCountDownLatch();
  assert(1 == 2);

  return 0;
}
