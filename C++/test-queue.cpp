/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * Test program for c++ snippest to try to out some features.
 */

#include <array>
#include <cstdio>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <list>
#include <queue>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
  list<int> l = {1, 2, 3};
  queue<int, list<int>> q(l);

  while (!q.empty()) {
    cout << q.front() << endl;
    q.pop();
  }

  return 0;
}
