/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program demonstrates the implementation of
 * ring buffer that:
 *
 * - the ring buffer can store 50 ints at the beginning.
 *
 * - there are 2 writers running continuously in background
 *   to write a sequence of numbers into the ring buffer. Each
 *   writer will run for 60s and it takes 500ms to generate
 *   the next number to write to ring buffer.
 *
 * - there is one reader running continouusly in background
 *   to read a sequence of numbers from the ring buffer. The
 *   reader will run for 280 or no data to read for 10s,
 *   it takes 1 second to process the number it reads from ring buffer.
 *
 * the above setup allows the ringbuffer to be filled relative quickly,
 * and the writer will step up the ringbuffer size instead of wait
 * for free slot, this allow writer thread to continue to write without
 * pausing.
 */

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

using std::chrono::system_clock;

struct ringbuffer_t {
  std::condition_variable w_cv{};
  std::condition_variable r_cv{};
  std::condition_variable cv{};
  std::mutex mutex{};

  std::vector<int> buffer{std::vector<int>(50, 0)};
  std::size_t size{buffer.size()};
  std::size_t stepup_size{20};
  std::size_t max_size{1000};

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

      while (ringbuffer->r_index == ringbuffer->w_index) {
        auto cv_ret = ringbuffer->r_cv.wait_for(lck, std::chrono::seconds(10));
        if (std::cv_status::timeout == cv_ret) {
          goto end;
        }
      }

      num = ringbuffer->buffer[ringbuffer->r_index];
      ringbuffer->r_index = (ringbuffer->r_index + 1) % ringbuffer->size;

      ringbuffer->w_cv.notify_all();
    }

    try {
      dict.at(num) += 1;
    } catch (const std::out_of_range &oor) {
      dict[num] = 1;
    }

    count++;
    tt = system_clock::to_time_t(system_clock::now());
    std::this_thread::sleep_for(std::chrono::seconds(1));
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

      int next_w_index = (ringbuffer->w_index + 1) % ringbuffer->size;

      while (next_w_index == ringbuffer->r_index) {
        if (ringbuffer->size >= ringbuffer->max_size) {
          std::cout << "pause w_thread " << std::this_thread::get_id() << "\n";

          auto cv_ret = ringbuffer->w_cv.wait_for(
              lck, std::chrono::seconds(std::max(1L, next_tt - tt)));
          if (std::cv_status::timeout == cv_ret) {
            goto end;
          }
        } else {
          // this can be expensive if the reader thread is too slow, example is
          // that we have a ringbuffer of 50, and reader next read is at index
          // 4, and this writer thread next write is 4, and we have to move 4 to
          // 49 toward to newly added buffer at the end.

          std::size_t new_size{
              std::min(ringbuffer->max_size,
                       ringbuffer->size + ringbuffer->stepup_size)};

          std::cout << "writer thread " << std::this_thread::get_id()
                    << " extends ringbuffer size from " << ringbuffer->size
                    << " to " << new_size << "\n";
          ringbuffer->buffer.resize(new_size, 0);
          int nr_to_move = ringbuffer->size - ringbuffer->r_index;

          // important to move from backward in case that the newly added nr of
          // elements is smaller than nr_to_move.
          for (int i = 0; i < nr_to_move; i++) {
            ringbuffer->buffer[new_size - 1 - i] =
                ringbuffer->buffer[ringbuffer->size - 1 - i];
          }

          ringbuffer->r_index = new_size - nr_to_move;
          ringbuffer->size = new_size;
        }
      }

      ringbuffer->buffer[ringbuffer->w_index] = initial_value;
      ringbuffer->w_index = next_w_index;

      ringbuffer->r_cv.notify_all();
    }

    initial_value++;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    tt = system_clock::to_time_t(system_clock::now());
  }

end:
  // always lock prior std::cout, so the message is not messed up
  {
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
  std::thread w3(reader_fn, &ringbuffer, 240);

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
