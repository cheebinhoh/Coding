/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 */

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

void printVector(const std::vector<int> &v) {
  int cnt;

  cnt = 0;
  for (const auto i : v) {
    if (cnt != 0) {
      std::cout << " ";
    }

    std::cout << i;
    cnt++;
  }

  std::cout << "\n";
}

int main(int argc, char *argv[]) {
  std::vector<int> v1 = {1, 2, 3, 4, 5};

  std::cout << "vector with 5 elements: ";
  printVector(v1);
  std::cout << "\n";

  v1.resize(7);
  std::cout << "resize vector to 7 elements with value initializer: ";
  printVector(v1);
  std::cout << "\n";

  v1.resize(4);
  std::cout << "resize vector to 4 elements with value initializer: ";
  printVector(v1);
  std::cout << "\n";

  v1.resize(6, 5);
  std::cout << "resize vector to 4 elements with value 5: ";
  printVector(v1);
  std::cout << "\n";

  std::vector<int>::iterator end = v1.end();
  end--;
  v1.erase(end);
  std::cout << "delete last element (6th) of vector: ";
  printVector(v1);
  std::cout << "\n";

  int cnt = 0;
  std::vector<int>::iterator iter = v1.begin();
  while (iter != v1.end()) {
    if (*iter % 2 == 0) {
      cnt++;
      v1.erase(iter);
    }

    iter++;
  }

  std::cout << "Number of even values is " << cnt << ", ";
  std::cout << "delete even value from vector: ";
  printVector(v1);
  std::cout << "\n";

  cnt = 0;
  std::remove_if(v1.begin(), v1.end(), [&cnt](int i) mutable {
    if (i != 3) {
      cnt++;
    }

    return i == 3;
  });

  // is it a bug that std::remove_if does not shrink the vector to reflect new
  // size of elements? we purposely resize it after calling std::remove_if().
  v1.resize(cnt);
  std::cout << "Remove 3 from vector, it has " << cnt << " elements left: ";
  printVector(v1);
  std::cout << "\n";

  return 0;
}
