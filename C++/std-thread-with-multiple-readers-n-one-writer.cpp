/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program demonstrates simple implementation of multiple read
 * and single write lock (part of POSIX pthread in C) in c++ std::thread,
 * std::condition_variable and std::mutex.
 */

#include <array>
#include <chrono>
#include <condition_variable>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

using std::chrono::system_clock;
using fn_t = std::function<void()>;

void do_func(int seconds, fn_t prefn, fn_t fn, fn_t postfn) {
  int mins{seconds / 60};

  if (nullptr != prefn) {
    prefn();
  }

  seconds = {seconds % 60};

  std::time_t tt = system_clock::to_time_t(system_clock::now());
  struct std::tm *ptm = std::localtime(&tt);

  ptm->tm_min += mins;
  ptm->tm_sec += seconds;

  std::time_t next_tt = mktime(ptm);
  tt = system_clock::to_time_t(system_clock::now());
  while (tt < next_tt) {
    if (nullptr != fn) {
      fn();

      std::this_thread::yield();
    }

    tt = system_clock::to_time_t(system_clock::now());
  }

  if (nullptr != postfn) {
    postfn();
  }
}

int main(int argc, char *argv[]) {
  int write_cnt{0};
  bool write_in_progress{false};
  bool write_is_waiting{false};
  int read_in_progress_cnt{0};
  std::condition_variable condv{};
  std::mutex mutex{};
  int i{0};

  std::cout << "# of hardware concurrency = "
            << std::thread::hardware_concurrency() << "\n";

  std::array<std::thread, 5> readers;
  std::array<int, 5> reader_cnt{0};
  for (i = 0; i < readers.size(); i++) {
    readers[i] = std::thread(
        do_func, 5, [&reader_cnt, i]() { reader_cnt[i] = 0; },
        [&reader_cnt, i, &mutex, &condv, &write_in_progress, &write_is_waiting,
         &read_in_progress_cnt]() {
          {
            std::unique_lock<std::mutex> lck(mutex);
            while (write_in_progress) {
              condv.wait(lck);
            }

            // one of the problems in multiple reads and single write is that a
            // stream of continuous read lock will push out pending wait lock,
            // this might not happen in many configuration because read lock
            // will execute actions that blocking it after it gives up the
            // read-write lock and giving the write thread a chance to run.
            //
            // but in the case that a continuous run of read threads is to just
            // crunch some number without context switching, the write thread
            // will be pushed out and keep waiting for read thread count to
            // reach zero, which might not happen often and depend on the mercy
            // of CPU scheduling.
            //
            // the extra condition (write_is_waiting) is a simple trick to
            // prevrent that.
            while (write_is_waiting) {
              condv.wait(lck);
            }

            read_in_progress_cnt++;
          }

          reader_cnt[i]++;
          std::this_thread::sleep_for(std::chrono::milliseconds(100));

          {
            std::unique_lock<std::mutex> lck(mutex);
            read_in_progress_cnt--;
            condv.notify_all();
          }
        },
        nullptr);
  }

  do_func(
      5, [&write_cnt]() { write_cnt = 0; },
      [&write_cnt, &mutex, &condv, &write_in_progress, &write_is_waiting,
       &read_in_progress_cnt]() {
        {
          std::unique_lock<std::mutex> lck(mutex);
          while (read_in_progress_cnt > 0) {
            write_is_waiting = true;
            condv.wait(lck);
            write_is_waiting = false;
          }

          write_in_progress = true;
        }

        write_cnt++;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        {
          std::unique_lock<std::mutex> lck(mutex);
          write_in_progress = false;
          condv.notify_all();
        }
      },
      [&write_cnt]() {
        std::cout << "write thread runs " << write_cnt << " times \n";
      });

  i = 0;
  for (auto &th : readers) {
    th.join();
    std::cout << "read thread[" << i << "] runs " << reader_cnt[i]
              << " times\n";
    i++;
  }

  return 0;
}
