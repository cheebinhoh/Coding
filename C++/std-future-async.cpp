/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program demonstrates std::future and std::async:
 * - a asynchronous thread (std::async) is doing expensive
 *   sorting in background, and synchronous its sorting through
 *   std::future object.
 *
 * - the main thread will wait for the sorting to be done and
 *   periodically wake up and print "." to indicate the progress.
 */

#include <array>
#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>

template <typename T, std::size_t size>
void printArray(std::array<T, size> &arr, int upto = size) {
  int cnt = 0;

  for (auto &i : arr) {
    if (cnt++ == upto)
      break;

    std::cout << i << ", ";
  }
}

template <typename T, std::size_t size>
bool doSortArray(std::array<T, size> &arr) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        auto tmp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = tmp;
      }
    }
  }

  return true;
}

bool doSortIntArray(int *arr, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        auto tmp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = tmp;
      }
    }
  }

  return true;
}

int main(int argc, char *argv[]) {
  std::array<int, 30000> array{};

  std::generate(array.begin(), array.end(),
                []() { return std::rand() % 80000; });

  std::cout << "Array before explicilty sorted: ";
  printArray(array, 10);
  std::cout << "\n";
  std::cout << "Array is "
            << (std::is_sorted(array.begin(), array.end()) ? "" : "not ")
            << "sorted\n";
  std::cout << "\n";

  std::future<bool> sort_process =
      std::async(doSortIntArray, array.data(), array.size());
  // std::sort(array.begin(), array.end());
  // doSortIntArray(array.data(), array.size());

  std::cout << "sorting, please wait ";
  std::chrono::milliseconds span(100);
  while (sort_process.wait_for(span) == std::future_status::timeout) {
    std::cout << "." << std::flush;
  }

  std::cout << " done.\n";
  std::cout << "Array after explicitly sorted: ";
  bool sorted = sort_process.get();
  printArray(array, 10);
  std::cout << "\n";
  std::cout << "Array is " << (sorted ? "" : "not ") << "sorted\n";

  return 0;
}
