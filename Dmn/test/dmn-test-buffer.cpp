/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-pipe.hpp"
#include "dmn-proc.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <thread>

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  using namespace std::string_literals;

  auto buf = std::make_unique<Dmn::Dmn_Buffer<std::string>>();
  auto proc = std::make_unique<Dmn::Dmn_Proc>("proc", [&buf]() {
    static std::vector<std::string> result{"hello", "abc"};
    static int index {};

    auto s = buf->pop();
    EXPECT_TRUE(s == result[index++]);
    std::cout << "value pop: " << s << "\n";
  });

  std::string value{"hello"};

  buf->push(value);
  EXPECT_TRUE(value == "");

  proc->exec();

  Dmn::Dmn_Proc::yield();
  proc->wait();

  buf->push("abc"s);

  proc->exec();
  Dmn::Dmn_Proc::yield();
  proc->wait();

  proc->exec();
  Dmn::Dmn_Proc::yield();

  std::this_thread::sleep_for(std::chrono::seconds(2));
  proc = {};
  std::this_thread::sleep_for(std::chrono::seconds(2));
  buf = {};
  std::this_thread::sleep_for(std::chrono::seconds(3));

  Dmn::Dmn_Buffer<int> intBuf{};
  intBuf.push(2);

  EXPECT_TRUE(2 == intBuf.pop());

  Dmn::Dmn_Buffer<std::string> stringNotMoveBuf{};
  std::string stringToNotMoveBuf{"not move"};
  stringNotMoveBuf.push(stringToNotMoveBuf, false);
  std::string stringFromNotMoveBuf = stringNotMoveBuf.pop();

  EXPECT_TRUE("not move" == stringFromNotMoveBuf);
  EXPECT_TRUE("not move" == stringToNotMoveBuf);

  return RUN_ALL_TESTS();
}
