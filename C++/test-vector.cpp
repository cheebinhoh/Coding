/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * Test program for c++ snippest to try to out some features.
 */

#include <array>
#include <cstdio>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using namespace std;

template <typename T> void printVector(vector<T> vector) {
  int i;

  i = 0;
  for (auto val : vector) {
    if (i > 0)
      cout << ", ";

    cout << val;
    i++;
  }

  if (i > 0)
    cout << endl;
}

int main(int argc, char *argv[]) {
  vector<string> vstr = {"abc", "xyz", "c programming"};
  vector<int> vint = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
  array<int, 5> arr = {1, 2, 3};

  printVector(vstr);

  vstr.insert(vstr.end(), "swift");
  printVector(vstr);

  vstr.insert(vstr.begin(), {"Unix", "Linux"});
  printVector(vstr);

  for (auto i : arr)
    cout << i << endl;

  cout << "before sort integer vector" << endl;
  printVector(vint);
  cout << "after sort" << endl;
  sort(vint.begin(), vint.end());
  printVector(vint);

  return 0;
}
