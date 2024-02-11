#include "hal-buffer.hpp"
#include "hal-limit-buffer.hpp"
#include "hal-pipe.hpp"
#include "hal-proc.hpp"
#include "hal-teepipe.hpp"
#include <algorithm>
#include <iostream>

int main(int argc, char *argv[]) {
  Hal_TeePipe<int> sortPipe{
      "sortPipe", [](int v) { std::cout << "val: " << v << "\n"; },
      [](std::vector<int> list) { std::sort(list.begin(), list.end()); }};

  std::vector<int> s1{1, 3, 5, 7, 9};
  auto p1 = sortPipe.addHalTeePipeSource();
  Hal_Proc proc1{"s1", [&p1, &s1]() {
                   for (auto &v : s1) {
                     p1->push(v);
                   }
                 }};

  std::vector<int> s2{2, 4, 6, 8, 10};
  auto p2 = sortPipe.addHalTeePipeSource();
  Hal_Proc proc2{"s2", [&p2, &s2]() {
                   for (auto &v : s2) {
                     p2->push(v);
                   }
                 }};

  proc2.exec();
  proc1.exec();

  proc1.wait();
  proc2.wait();

  sortPipe.removeHalTeePipeSource(p1);
  sortPipe.removeHalTeePipeSource(p2);

  sortPipe.waitForEmpty();

  return 0;
}
