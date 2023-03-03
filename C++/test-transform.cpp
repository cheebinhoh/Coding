/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * Test program for c++ snippest to try to out some features.
 */

#include <cstdio>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

template <typename T> void printVector(vector<T> v, string sep) {
  if (v.empty())
    return;

  vector<int>::iterator it = v.begin();
  cout << *it++;

  while (it != v.end())
    cout << sep << *it++;

  cout << endl;
}

template <typename T> void printVector(vector<T> v) { printVector(v, " "); }

template <typename T> class Accummulator {
  T lastValue;
  Accummulator();

public:
  Accummulator(T initialValue);
  T operator()(T value);
  void test();
};

template <typename T>
Accummulator<T>::Accummulator(T initialValue) : lastValue(initialValue) {}

template <typename T> T Accummulator<T>::operator()(T value) {
  lastValue += value;

  return lastValue;
}

int main(int argc, char *argv[]) {
  vector<int> v1 = {1, 2, 3, 4, 5};
  vector<int> v2(v1.size());
  Accummulator<int> acc(0);
  int lastValue = 0;
  multiplies<int> m;

  printVector(v1);

  transform(v1.begin(), v1.end(), v2.begin(),
            [lastValue](int val) mutable -> int { return lastValue += val; });

  printVector(v2);

  cout << endl;
  cout << m(10, 20) << endl;

  return 0;
}
