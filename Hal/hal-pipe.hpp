/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements a fifo pipe that:
 * - write is not blocking
 * - read is blocking and std::nullptr_t if fifo pipe is destroyed
 * - readAndProcess will block and repeatedly run the functor to process each
 * data read out of the pipe.
 *
 * WARNING: write will move data (ownership) into the Hal_Pipe object upon
 * successful execution.
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

  std::optional<T> read() override {
    T data{};

    try {
      readAndProcess([&data](T &&item) { data = item; });
    } catch (...) {
      return {};
    }

    return std::move_if_noexcept(data);
  }

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

  void write(T &rItem) override { Hal_Buffer<T>::push(rItem, false); }

  void write(T &&rItem) override { Hal_Buffer<T>::push(rItem, true); }

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
