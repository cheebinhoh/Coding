/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program demonstrates std::array.
 */

#include <array>
#include <iostream>

int main(int argc, char *argv[]) {
  std::array<int, 5> arr1{1, 2, 3, 4, 5};
  int cnt;

  std::cout << "Original array values: ";
  cnt = 0;
  for (auto i : arr1) {
    std::cout << i << " ";

    arr1[cnt] = i * 10;

    cnt++;
  }

  std::cout << "\n";
  std::cout << "Change array values: ";
  for (auto i : arr1) {
    std::cout << i << " ";
  }

  std::cout << "\n";

  auto arr1_iter = arr1.rbegin();
  std::cout << "Reverse array values: ";
  while (arr1_iter != arr1.rend()) {
    std::cout << *arr1_iter << " ";

    arr1_iter++;
  }

  std::cout << "\n";

  arr1.fill(0);
  std::cout << "Reset array values to 0: ";
  for (auto i : arr1) {
    std::cout << i << " ";
  }

  std::cout << "\n";

  std::cout << "Size by end() - begin() = " << arr1.end() - arr1.begin()
            << "\n";
  std::cout << "\n";

  return 0;
}
