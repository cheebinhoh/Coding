/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * Test program for c++ snippest to try to out some features.
 */

#include <cstdio>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
  map<int, string> nmap = {{0, "Zero"}, {1, "One"}, {2, "Two"}};

  cout << nmap[0] << endl;

  return 0;
}
