/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements a fifo pipe that:
 * - write is not blocking
 * - a thread (via Hal_Proc) can be setup to process each item pop out from
 *   the fifo pipe.
 * - client can call read() to read the next item pop out of the fifo pipe,
 *   or blocked waiting for one if the fifo pipe is empty.
 * - client can call readAndProcess() to read the next item pop out of the
 *   fifo pipe and invoke the Task to process the item or blocked waiting
 *   for one if the fifo pipe is empty.
 */

#ifndef HAL_PIPE_HPP_HAVE_SEEN

#define HAL_PIPE_HPP_HAVE_SEEN

#include "hal-buffer.hpp"
#include "hal-io.hpp"
#include "hal-proc.hpp"

#include <cstring>
#include <functional>
#include <optional>
#include <pthread.h>

template <typename T>
class Hal_Pipe : public Hal_Buffer<T>, public Hal_Io<T>, public Hal_Proc {
  using Task = std::function<void(T &&)>;

public:
  Hal_Pipe(std::string_view name, Hal_Pipe::Task fn = {}) : Hal_Proc{name} {
    int err = pthread_mutex_init(&m_mutex, NULL);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    err = pthread_cond_init(&m_emptyCond, NULL);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    if (fn) {
      exec([this, fn]() {
        while (true) {
          readAndProcess(fn);
        }
      });
    }
  }

  virtual ~Hal_Pipe() noexcept try {
    // stopExec is not noexcept, so we need to resolve it in destructor
    Hal_Proc::stopExec();

    pthread_cond_signal(&m_emptyCond);
    pthread_cond_destroy(&m_emptyCond);
    pthread_mutex_destroy(&m_mutex);
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  Hal_Pipe(const Hal_Pipe<T> &halPipe) = delete;
  const Hal_Pipe<T> &operator=(const Hal_Pipe<T> &halPipe) = delete;
  Hal_Pipe(Hal_Pipe<T> &&halPipe) = delete;
  Hal_Pipe<T> &operator=(Hal_Pipe<T> &&halPipe) = delete;

  /**
   * @brief The method will read and return an item from the pipe or
   *        std::nullopt if the pipe is closed. The read is blocked waiting
   *        for next pop out of the pipe.
   *
   * @return optional item if there is next item from pipe, or std::nullopt
   *         if pipe is closed
   */
  std::optional<T> read() override {
    T data{};

    try {
      readAndProcess([&data](T &&item) { data = item; });
    } catch (...) {
      return {};
    }

    return std::move_if_noexcept(data);
  }

  /**
   * @brief The method read the next item pop out of pipe and call fn functor
   *        to process item, the method is blocked waiting for next item
   *        from the pipe.
   *
   * @param fn functor to process next item pop out of pipe
   */
  void readAndProcess(Hal_Pipe::Task fn) {
    T &&item = this->pop();

    int errInLoop = pthread_mutex_lock(&m_mutex);
    if (errInLoop) {
      throw std::runtime_error(strerror(errInLoop));
    }

    pthread_testcancel();

    fn(std::move_if_noexcept(item));

    ++m_count;

    errInLoop = pthread_cond_signal(&m_emptyCond);
    if (errInLoop) {
      pthread_mutex_unlock(&m_mutex);

      throw std::runtime_error(strerror(errInLoop));
    }

    pthread_testcancel();

    errInLoop = pthread_mutex_unlock(&m_mutex);
    if (errInLoop) {
      throw std::runtime_error(strerror(errInLoop));
    }
  }

  /**
   * @brief The method will write data into the pipe, the data is copied
   *        than move semantic.
   *
   * @param rItem The data item to be copied into pipe
   */
  void write(T &rItem) override { Hal_Buffer<T>::push(rItem, false); }

  /**
   * @brief The method will write data into the pipe, the data is moved
   *        into pipe if noexcept.
   *
   * @param rItem The data item to be moved into pipe
   */
  void write(T &&rItem) override { Hal_Buffer<T>::push(rItem, true); }

  /**
   * @brief The method will put the client on blocking wait until
   *        the pipe is empty and all items have been pop out and processed,
   *        it returns number of items that were passed through the pipe in
   *        total upon return.
   *
   * @return The number of items that were passed through the pipe
   *         in total
   */
  long long waitForEmpty() override {
    long long inboundCount{};

    inboundCount = Hal_Buffer<T>::waitForEmpty();

    int err = pthread_mutex_lock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    while (m_count < inboundCount) {
      err = pthread_cond_wait(&m_emptyCond, &m_mutex);
      if (err) {
        throw std::runtime_error(strerror(err));
      }

      pthread_testcancel();
    }

    err = pthread_mutex_unlock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    return inboundCount;
  }

private:
  using Hal_Buffer<T>::pop;
  using Hal_Buffer<T>::popNoWait;
  using Hal_Buffer<T>::push;
  pthread_mutex_t m_mutex{};
  pthread_cond_t m_emptyCond{};
  long long m_count{};
};

#endif /* HAL_PIPE_HPP_HAVE_SEEN */
