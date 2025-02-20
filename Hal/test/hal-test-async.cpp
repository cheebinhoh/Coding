#include "hal-async.hpp"
#include "hal-proc.hpp"

#include <gtest/gtest.h>
#include <iostream>
#include <thread>

class Counter : Hal_Async {
public:
  Counter() : Hal_Async{"counter"} {}

  void increment() {
    HAL_ASYNC_CALL_WITH_REF_CAPTURE({ this->count++; });
  }

  operator long long() { return count; }

private:
  long long count{};
};

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  Counter cnt{};
  Hal_Proc proc1{"proc1", [&cnt]() {
                   for (int n = 0; n < 100; n++) {
                     cnt.increment();
                     Hal_Proc::yield();
                   }
                 }};

  Hal_Proc proc2{"proc2", [&cnt]() {
                   for (int n = 0; n < 100; n++) {
                     cnt.increment();
                     Hal_Proc::yield();
                   }
                 }};

  proc1.exec();
  proc2.exec();
  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_TRUE(static_cast<long long>(cnt) == 200);

  return RUN_ALL_TESTS();
}
