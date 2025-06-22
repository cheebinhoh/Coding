/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <string>

#include "dmn-async.hpp"
#include "dmn-buffer.hpp"
#include "dmn-limit-buffer.hpp"
#include "dmn-pipe.hpp"
#include "dmn-proc.hpp"
#include "dmn-teepipe.hpp"
#include "dmn.hpp"

/* An example usage of Dmn async where we can wrap the api body within
 * dmn async call to serialize multiple api called from different threads,
 * and converging those calls into one thread ad and always have one
 * thread responsible to run api call that changes the object, this will
 * de-couple caller and callee and avoid mutex delay
 */
class Dmn_Event : public dmn::Dmn_Async {
public:
  Dmn_Event() : dmn::Dmn_Async{"event manager"} {}

  void post(std::string event) {
    DMN_ASYNC_CALL_WITH_CAPTURE(std::cout << "Event: " << count++ << ": "
                                          << event << "\n",
                                this, event);
  }

private:
  long count{};
};

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  dmn::Dmn_TeePipe<int> sortPipe{
      "sortPipe", [](int v) { std::cout << "val: " << v << "\n"; },
      [](std::vector<int> list) { std::sort(list.begin(), list.end()); }};

  std::vector<int> s1{1, 3, 5, 7, 9};
  auto p1 = sortPipe.addDmn_TeePipeSource();
  dmn::Dmn_Proc proc1{"s1", [&p1, &s1]() {
                        for (auto &v : s1) {
                          p1->write(v);
                        }
                      }};

  std::vector<int> s2{2, 4, 6, 8, 10};
  auto p2 = sortPipe.addDmn_TeePipeSource();
  dmn::Dmn_Proc proc2{"s2", [&p2, &s2]() {
                        for (auto &v : s2) {
                          p2->write(v);
                        }
                      }};

  proc2.exec();
  proc1.exec();

  proc1.wait();
  proc2.wait();

  sortPipe.removeDmn_TeePipeSource(p1);
  sortPipe.removeDmn_TeePipeSource(p2);

  sortPipe.waitForEmpty();

  dmn::Dmn_Async ha{"Test"};
  std::function<void()> functor{
      []() { std::cout << "Executing the functor\n"; }};

  ha.write(functor);
  ha.waitForEmpty();

  using namespace std::string_literals;

  Dmn_Event e{};
  e.post("Hello event 1"s);
  e.post("Hello event 2"s);
  e.post("Hello event 3"s);

  std::cout << "after post event\n";
  e.waitForEmpty();

  std::cout << "End of Dmn test\n";

  {
    dmn::Dmn_Proc nonestop{"none-stop", []() {
                             std::cout << "start none-stop\n";
                             while (true) {
                               dmn::Dmn_Proc::yield();
                             }
                           }};
    nonestop.exec();
    dmn::Dmn_Proc::yield();
  }

  dmn::Dmn_Pipe<std::string> pipe{"string"};
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

  auto pipeBlock = std::make_unique<dmn::Dmn_Pipe<std::string>>("pipeBlock");
  dmn::Dmn_Proc pipeBlockProc{"pipeBlockProc", [&pipeBlock]() {
                                std::this_thread::sleep_for(
                                    std::chrono::seconds(5));
                                pipeBlock = {};
                              }};

  pipeBlockProc.exec();
  dmn::Dmn_Proc::yield();
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
