#ifndef HAL_LIMITBUFFER_HPP_HAVE_SEEN

#define HAL_LIMITBUFFER_HPP_HAVE_SEEN

#include "hal-buffer.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <deque>
#include <optional>

#include <pthread.h>

template <typename T> class Hal_LimitBuffer : private Hal_Buffer<T> {
public:
  Hal_LimitBuffer(size_t capacity = 1) : m_maxCapacity(capacity) {
    int err{};

    err = pthread_mutex_init(&m_mutex, NULL);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    err = pthread_cond_init(&m_pushCond, NULL);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    err = pthread_cond_init(&m_popCond, NULL);
    if (err) {
      throw std::runtime_error(strerror(err));
    }
  }

  virtual ~Hal_LimitBuffer() {
    pthread_cond_destroy(&m_pushCond);
    pthread_cond_destroy(&m_popCond);
    pthread_mutex_destroy(&m_mutex);
  }

  Hal_LimitBuffer(const Hal_LimitBuffer<T> &halLimitBuffer) = delete;
  const Hal_LimitBuffer<T> &
  operator=(const Hal_LimitBuffer<T> &halLimitBuffer) = delete;
  Hal_LimitBuffer(Hal_LimitBuffer<T> &&halLimitBuffer) = delete;
  Hal_LimitBuffer<T> &&operator=(Hal_LimitBuffer<T> &&halLimitBuffer) = delete;

  void push(T &rItem) {
    int err{};

    err = pthread_mutex_lock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    while (m_size >= m_maxCapacity) {
      err = pthread_cond_wait(&m_pushCond, &m_mutex);
      if (err) {
        throw std::runtime_error(strerror(err));
      }

      pthread_testcancel();
    }

    Hal_Buffer<T>::push(rItem);

    ++m_size;

    err = pthread_cond_signal(&m_popCond);
    if (err) {
      pthread_mutex_unlock(&m_mutex);

      throw std::runtime_error(strerror(err));
    }

    err = pthread_mutex_unlock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }
  }

  size_t size() {
    int err{};
    size_t size{};

    err = pthread_mutex_lock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    size = m_size;

    err = pthread_mutex_unlock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    return size;
  }

  T pop() { return *pop(true); }

  std::optional<T> popNoWait() { return pop(false); }

  long long waitForEmpty() { return Hal_Buffer<T>::waitForEmpty(); }

private:
  std::optional<T> pop(bool wait) {
    int err{};

    err = pthread_mutex_lock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    if (0 == m_size) {
      if (!wait) {
        err = pthread_mutex_unlock(&m_mutex);
        if (err) {
          throw std::runtime_error(strerror(err));
        }

        return {};
      }

      do {
        err = pthread_cond_wait(&m_popCond, &m_mutex);
        if (err) {
          throw std::runtime_error(strerror(err));
        }

        pthread_testcancel();
      } while (0 == m_size);
    }

    T val = Hal_Buffer<T>::pop();
    m_size--;

    err = pthread_cond_signal(&m_pushCond);
    if (err) {
      pthread_mutex_unlock(&m_mutex);

      throw std::runtime_error(strerror(err));
    }

    err = pthread_mutex_unlock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    return val; // val is local variable, hence rvalue and hence move semantic
                // by default for efficient copy.
  }

private:
  size_t m_maxCapacity{1};
  size_t m_size{0};
  pthread_mutex_t m_mutex{};
  pthread_cond_t m_popCond{};
  pthread_cond_t m_pushCond{};
};

#endif /* HAL_LIMITBUFFER_HPP_HAVE_SEEN */
