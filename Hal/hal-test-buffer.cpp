#include "hal-pipe.hpp"
#include "hal-proc.hpp"

#include <iostream>
#include <memory>
#include <thread>

int main(int argc, char *argv[]) {
  using namespace std::string_literals;

  auto buf = std::make_unique<Hal_Buffer<std::string>>();
  auto proc = std::make_unique<Hal_Proc>("proc", [&buf]() {
    auto s = buf->pop();
    std::cout << "value pop: " << s << "\n";
  });

  std::string value{"hello"};

  buf->push(value);

  proc->exec();

  Hal_Proc::yield();
  proc->wait();

  buf->push("abc"s);

  proc->exec();
  Hal_Proc::yield();
  proc->wait();

  proc->exec();
  Hal_Proc::yield();

  std::this_thread::sleep_for(std::chrono::seconds(2));
  proc = {};
  std::this_thread::sleep_for(std::chrono::seconds(2));
  buf = {};
  std::this_thread::sleep_for(std::chrono::seconds(3));

  Hal_Buffer<int> intBuf{};
  intBuf.push(2);

  std::cout << "pop int: " << intBuf.pop() << "\n";

  return 0;
}
