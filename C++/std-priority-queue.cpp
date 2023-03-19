/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This is to demonstrate some features of std::priority_queue
 * container adaptor.
 */

#include <iostream>
#include <queue>

int main(int argc, char *argv[]) {
  std::priority_queue<int> pq{};

  pq.push(5);
  pq.push(2);
  pq.push(3);
  pq.push(1);
  pq.push(4);

  while (false == pq.empty()) {
    auto top = pq.top();

    std::cout << top << " ";

    pq.pop();
  }

  std::cout << "\n";

  return 0;
}
