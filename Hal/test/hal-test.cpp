/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include "hal-async.hpp"
#include "hal-buffer.hpp"
#include "hal-limit-buffer.hpp"
#include "hal-pipe.hpp"
#include "hal-proc.hpp"
#include "hal-teepipe.hpp"
#include "hal.hpp"

#include <gtest/gtest.h>
#include <algorithm>
#include <iostream>
#include <string>

/* An example usage of hal async where we can wrap the api body within
 * hal async call to serialize multiple api called from different threads,
 * and converging those calls into one thread ad and always have one
 * thread responsible to run api call that changes the object, this will
 * de-couple caller and callee and avoid mutex delay
 */
class Hal_Event : public Hal_Async {
public:
  Hal_Event() : Hal_Async{"event manager"} {}

  void post(std::string event) {
    HAL_ASYNC_CALL_WITH_COPY_CAPTURE(std::cout << "Event: " << count++ << ": "
                                               << event << "\n");
  }

private:
  long count{};
};

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  Hal_TeePipe<int> sortPipe{
      "sortPipe", [](int v) { std::cout << "val: " << v << "\n"; },
      [](std::vector<int> list) { std::sort(list.begin(), list.end()); }};

  std::vector<int> s1{1, 3, 5, 7, 9};
  auto p1 = sortPipe.addHal_TeePipeSource();
  Hal_Proc proc1{"s1", [&p1, &s1]() {
                   for (auto &v : s1) {
                     p1->write(v);
                   }
                 }};

  std::vector<int> s2{2, 4, 6, 8, 10};
  auto p2 = sortPipe.addHal_TeePipeSource();
  Hal_Proc proc2{"s2", [&p2, &s2]() {
                   for (auto &v : s2) {
                     p2->write(v);
                   }
                 }};

  proc2.exec();
  proc1.exec();

  proc1.wait();
  proc2.wait();

  sortPipe.removeHal_TeePipeSource(p1);
  sortPipe.removeHal_TeePipeSource(p2);

  sortPipe.waitForEmpty();

  Hal_Async ha{"Test"};
  std::function<void()> functor{
      []() { std::cout << "Executing the functor\n"; }};

  ha.write(functor);
  ha.waitForEmpty();

  using namespace std::string_literals;

  Hal_Event e{};
  e.post("Hello event 1"s);
  e.post("Hello event 2"s);
  e.post("Hello event 3"s);

  std::cout << "after post event\n";
  e.waitForEmpty();

  std::cout << "End of Hal test\n";

  {
    Hal_Proc nonestop{"none-stop", []() {
                        std::cout << "start none-stop\n";
                        while (true) {
                          Hal_Proc::yield();
                        }
                      }};
    nonestop.exec();
    Hal_Proc::yield();
  }

  Hal_Pipe<std::string> pipe{"string"};
  std::string valToPipe{"Hello Pipe"};
  std::string valFromPipe{};

  pipe.write(valToPipe);
  pipe.readAndProcess([&valFromPipe](std::string &&val) { valFromPipe = val; });
  std::cout << "value from Pipe: " << valFromPipe
            << ", value to Pipe: " << valToPipe << "\n";

  valToPipe = "Hello Pipe 2";
  pipe.write(valToPipe);
  auto optionalValFromPipe = pipe.read();
  std::cout << "value from Pipe: " << (*optionalValFromPipe)
            << ", value to Pipe: " << valToPipe << "\n";

  auto pipeBlock = std::make_unique<Hal_Pipe<std::string>>("pipeBlock");
  Hal_Proc pipeBlockProc{"pipeBlockProc", [&pipeBlock]() {
                           std::this_thread::sleep_for(std::chrono::seconds(5));
                           pipeBlock = {};
                         }};

  pipeBlockProc.exec();
  Hal_Proc::yield();
  std::this_thread::sleep_for(std::chrono::seconds(2));

  std::cout << "before pipeBlock->read\n";
  auto optVal = pipeBlock->read();
  if (!optVal) {
    std::cout << "no data\n";
  }

  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "end\n";

  return RUN_ALL_TESTS();
}
