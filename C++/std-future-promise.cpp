/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program demonstrates std::future and std::promise:
 * - a asynchronous thread is explicitly created to do expensive
 *   sorting in background, and synchronous its sorting through
 *   std::promise object.
 *
 * - the main thread will wait for the sorting to be done and
 *   periodically wake up and print "." to indicate the progress
 *   through std::future object.
 *
 * the relationship between various key parts of std::future can
 * be viewed as following:
 *
 * std::promise -> std::packaged_task -> std::async
 */

#include <array>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <future>
#include <iomanip>
#include <iostream>
#include <thread>

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

void thread_do_sort(std::promise<bool> &prom, int *arr, int size) {
  doSortIntArray(arr, size);

  prom.set_value(true);
}

int main(int argc, char *argv[]) {
  using std::chrono::system_clock;

  std::array<int, 50000> array{};

  std::generate(array.begin(), array.end(),
                []() { return std::rand() % 80000; });

  std::cout << "Array of (" << array.size() << ") before explicilty sorted: ";
  printArray(array, 10);
  std::cout << "\n";
  std::cout << "Array is "
            << (std::is_sorted(array.begin(), array.end()) ? "" : "not ")
            << "sorted\n";
  std::cout << "\n";

  std::time_t tt = system_clock::to_time_t(system_clock::now());
  struct std::tm *ptm = std::localtime(&tt);
  std::cout << "Current time before run sorting is " << std::put_time(ptm, "%X")
            << "\n";

  std::promise<bool> prom;
  std::future<bool> sort_process = prom.get_future();
  std::thread sort_process_th =
      std::thread(thread_do_sort, std::ref(prom), array.data(), array.size());

  std::cout << "sorting, please wait ";
  std::chrono::milliseconds span(100);
  while (sort_process.wait_for(span) == std::future_status::timeout) {
    std::cout << "." << std::flush;
  }

  bool sorted = sort_process.get();

  tt = system_clock::to_time_t(system_clock::now());
  ptm = std::localtime(&tt);

  std::cout << "\n";
  std::cout << "Current time after done sorting is " << std::put_time(ptm, "%X")
            << "\n";

  std::cout << "\n";
  std::cout << "Array of (" << array.size() << ") after explicitly sorted: ";
  printArray(array, 10);
  std::cout << "\n";
  std::cout << "Array is "
            << (std::is_sorted(array.begin(), array.end()) ? "" : "not ")
            << "sorted\n";

  std::cout << "\n";

  sort_process_th.join();

  return 0;
}
