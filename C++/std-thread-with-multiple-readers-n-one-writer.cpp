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

/* write_stagnant_avoidance { true } will help avoid write stagnant when
 * a continuous stream of read threads occurs and push out the waiting write
 * thread. An example is illustrated below.
 *
 * on my machine (macOS), the std::thread::hardware_concurrency() = 16.
 *
 * if num of read threads are 16 and write stagnant avoidance is enable, write
 * thread has about 8 runs, and all other read thread has average between 10 -
 * 15 runs. If write stagnant avoidance is disable, write thread has 1 run, and
 * the all other read threads have average 47 run. As the # of read threads
 * decreased in relative to the hardware concurrency, there is more chance that
 * write thread got to execute even we do not have write stagnant avoidance be
 * true.
 *
 * As the number of read threads increased and catching up to the hardware
 * concurrency, the important of write stagnant avoidance become significant
 * to prevent any form of stagnant.
 */
const int run_duration_second{5};
const int write_stagnant_avoidance{true};
const int num_of_reads{16};

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
  int i{0};
  int write_cnt{0};
  int read_in_progress_cnt{0};
  bool read_has_data{false};
  bool write_in_progress{false};
  bool write_is_waiting{false};
  std::condition_variable condv{};
  std::mutex mutex{};

  std::cout << "# of hardware concurrency = "
            << std::thread::hardware_concurrency() << "\n";

  std::array<std::thread, num_of_reads> readers;
  std::array<int, num_of_reads> reader_cnt{0};
  for (i = 0; i < readers.size(); i++) {
    readers[i] = std::thread(
        do_func, run_duration_second, [&reader_cnt, i]() { reader_cnt[i] = 0; },
        [&reader_cnt, i, &mutex, &condv, &write_in_progress, &write_is_waiting,
         &read_in_progress_cnt, &read_has_data]() {
          {
            std::unique_lock<std::mutex> lck(mutex);
            while (write_in_progress || !read_has_data) {
              condv.wait(lck);
            }

            // One of the problems in multiple reads and single write is that a
            // stream of continuous read lock will push out pending wait lock,
            // this might not happen in many configuration because read lock
            // will execute actions that blocking it after it gives up the
            // read-write lock and giving the write thread a chance to run.
            //
            // But in the case that a continuous run of read threads is to just
            // crunch some number without context switching, the write thread
            // will be pushed out and keep waiting for read thread count to
            // reach zero, which might not happen often and depend on the mercy
            // of CPU scheduling.
            //
            // the extra condition (write_is_waiting) is a simple trick to
            // prevrent that.
            while (write_is_waiting && write_stagnant_avoidance) {
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
      run_duration_second,
      [&write_cnt, &mutex, &condv, &read_has_data]() {
        write_cnt = 0;
        std::unique_lock<std::mutex> lck(mutex);
        read_has_data = true;
        condv.notify_all();
      },
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
