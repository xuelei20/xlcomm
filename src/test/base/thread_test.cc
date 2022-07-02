// 线程单元测试
// Author: xuel

#include <iostream>

#include "xlbase/thread/i_thread.h"

void TestCountDownLatch() {
  xlbase::CountDownLatch latch(3);

  std::thread thread_master([&latch]() {
    std::cout << "Init something\n";
    latch.Wait();
    std::cout << "Work finish\n";
  });

  std::thread thread_slave1([&latch]() {
    std::cout << "do work1\n";
    latch.CountDown();
  });

  std::thread thread_slave2([&latch]() {
    std::cout << "do work2\n";
    latch.CountDown();
  });

  std::thread thread_slave3([&latch]() {
    std::cout << "do work3\n";
    latch.CountDown();
  });

  thread_master.join();
  thread_slave1.join();
  thread_slave2.join();
  thread_slave3.join();
}

int main() {
  std::cout << "====thread test====\n";

  TestCountDownLatch();

  return 0;
}
