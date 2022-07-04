// 线程相关单元测试
// Author: xuel

#include "xlcomm/base/count_down_latch.h"

#include <assert.h>

#include <thread>
#include <iostream>

#define BOOST_TEST_MAIN
#include "boost/test/included/unit_test.hpp"

BOOST_AUTO_TEST_CASE(CountDownLatch) {
  xlcomm::base::CountDownLatch latch(3);
  std::cout << "sizeof(latch):" << sizeof(latch) << std::endl;

  std::thread thread_master([&latch]() {
    latch.Wait();
    BOOST_CHECK(latch.count() == 0);
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

BOOST_AUTO_TEST_CASE(mytest) {
  BOOST_CHECK(1 == 1);
  BOOST_CHECK(1 == 3);
}