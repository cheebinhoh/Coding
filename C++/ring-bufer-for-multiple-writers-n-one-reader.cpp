/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program demonstrates the implementation of
 * ring buffer that:
 *
 * - the ring buffer can store 500 ints at a time.
 *
 * - there are 3 writers running continuously in background
 *   to write a sequence of numbers into the ring buffer. Each
 *   writer will run for 60s and it takes 200ms to generate
 *   the next number to write to ring buffer
 *
 * - there is one reader running continouusly in background
 *   to read a sequence of numbers from the ring buffer. The
 *   reader will run for 120s, and it takes 400ms second to
 *   process the number it reads from ring buffer.
 *
 * - in this version if the ring buffer is fill without free
 *   slot, the writer will pause and wait for reader to catch up.
 */

#include <algorithm>
#include <array>
#include <chrono>
#include <condition_variable>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

using std::chrono::system_clock;

struct ringbuffer_t {
  std::condition_variable w_cv{};
  std::condition_variable r_cv{};
  std::condition_variable cv{};
  std::mutex mutex{};

  std::array<long, 500> buffer{};
  int r_index{0};
  int w_index{0};

  bool pause{true};
};

void reader_fn(struct ringbuffer_t *ringbuffer, int seconds) {
  long count{0};
  std::map<long, int> dict{};
  std::time_t tt = system_clock::to_time_t(system_clock::now());
  struct std::tm *ptm = std::localtime(&tt);

  ptm->tm_min += seconds / 60;
  ptm->tm_sec += seconds % 60;

  std::time_t next_tt = mktime(ptm);
  tt = system_clock::to_time_t(system_clock::now());
  while (tt < next_tt) {
    long num{0};

    {
      std::unique_lock<std::mutex> lck(ringbuffer->mutex);
      while (ringbuffer->pause) {
        ringbuffer->cv.wait(lck);
      }

      int next_r_index = (ringbuffer->r_index + 1) % ringbuffer->buffer.size();
      while (ringbuffer->r_index == ringbuffer->w_index) {
        auto cv_ret = ringbuffer->r_cv.wait_for(
            lck, std::chrono::seconds(std::max(1L, next_tt - tt)));
        if (std::cv_status::timeout == cv_ret) {
          goto end;
        }
      }

      num = ringbuffer->buffer[ringbuffer->r_index];
      ringbuffer->r_index =
          (ringbuffer->r_index + 1) % ringbuffer->buffer.size();

      ringbuffer->w_cv.notify_all();
    }

    try {
      dict.at(num) += 1;
    } catch (const std::out_of_range &oor) {
      dict[num] = 1;
    }

    count++;
    tt = system_clock::to_time_t(system_clock::now());

    std::this_thread::yield();
  }

end:
  // for (auto & me : dict) {
  //   std::cout << "value = " << me.first << " has count = " << me.second <<
  //   "\n";
  // }

  {
    std::unique_lock<std::mutex> lck(ringbuffer->mutex);

    std::cout << "reader thread id = " << std::this_thread::get_id()
              << ", last value = " << count << "\n";
  }
}

void writer_fn(struct ringbuffer_t *ringbuffer, long initial_value,
               int seconds) {
  std::time_t tt = system_clock::to_time_t(system_clock::now());
  struct std::tm *ptm = std::localtime(&tt);

  ptm->tm_min += seconds / 60;
  ptm->tm_sec += seconds % 60;

  std::time_t next_tt = mktime(ptm);
  tt = system_clock::to_time_t(system_clock::now());
  while (tt < next_tt) {
    {
      std::unique_lock<std::mutex> lck(ringbuffer->mutex);
      while (ringbuffer->pause) {
        ringbuffer->cv.wait(lck);
      }

      int next_w_index = (ringbuffer->w_index + 1) % ringbuffer->buffer.size();

      while (next_w_index == ringbuffer->r_index) {
        auto cv_ret = ringbuffer->w_cv.wait_for(
            lck, std::chrono::seconds(std::max(1L, next_tt - tt)));
        if (std::cv_status::timeout == cv_ret) {
          goto end;
        }
      }

      ringbuffer->buffer[ringbuffer->w_index] = initial_value;
      ringbuffer->w_index = next_w_index;

      ringbuffer->r_cv.notify_all();
    }

    initial_value++;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    tt = system_clock::to_time_t(system_clock::now());
  }

end : {
  std::unique_lock<std::mutex> lck(ringbuffer->mutex);
  std::cout << "writer thread id = " << std::this_thread::get_id()
            << ", last value = " << initial_value - 1 << "\n";
}
}

int main(int argc, char *argv[]) {
  struct ringbuffer_t ringbuffer {};

  std::time_t tt = system_clock::to_time_t(system_clock::now());
  struct std::tm *ptm = std::localtime(&tt);
  std::cout << "Current time before run sorting is " << std::put_time(ptm, "%X")
            << "\n";

  std::thread w1(writer_fn, &ringbuffer, 1, 60);
  std::thread w2(writer_fn, &ringbuffer, 1, 60);
  std::thread w3(reader_fn, &ringbuffer, 80);

  {
    std::unique_lock<std::mutex> lck(ringbuffer.mutex);
    ringbuffer.pause = false;
    ringbuffer.cv.notify_all();
  }

  w1.join();
  w2.join();
  w3.join();

  tt = system_clock::to_time_t(system_clock::now());
  ptm = std::localtime(&tt);
  std::cout << "Current time before run sorting is " << std::put_time(ptm, "%X")
            << "\n";

  return 0;
}
