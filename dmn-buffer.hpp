/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-buffer.hpp
 * @brief The header file for dmn-buffer which implements an fifo queue that:
 *        - push is not blocking
 *        - pop is blocking (optional) if no data in the queue
 *
 *        There is no programmatic limit on the size of the queue,
 *        but runtime memory limit or size type of the underlying
 *        STL class it uses.
 */

#ifndef DMN_BUFFER_HPP_

#define DMN_BUFFER_HPP_

#include <pthread.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <deque>
#include <optional>
#include <stdexcept>

#include "dmn-proc.hpp"

namespace dmn {

template <typename T> class Dmn_Buffer {
public:
  Dmn_Buffer();
  virtual ~Dmn_Buffer() noexcept;

  Dmn_Buffer(const Dmn_Buffer<T> &obj) = delete;
  const Dmn_Buffer<T> &operator=(const Dmn_Buffer<T> &obj) = delete;
  Dmn_Buffer(const Dmn_Buffer<T> &&obj) = delete;
  Dmn_Buffer<T> &operator=(Dmn_Buffer<T> &&dmnBuffer) = delete;

  /**
   * @brief The method will pop and return front item from the queue or the
   *        caller is blocked waiting if the queue is empty.
   *
   * @return front item of the queue
   */
  virtual T pop();

  /**
   * @brief The method will pop and return front item from the queue or the
   *        std::nullopt if the queue is empty (and caller is not blocked
   *        waiting).
   *
   * @return optional item from the front of the queue
   */
  virtual std::optional<T> popNoWait();

  /**
   * @brief The method will push the item into queue using move semantics
   *        unless noexcept is false.
   *
   * @param item The item to be pushed into queue
   */
  virtual void push(T &&item);

  /**
   * @brief The method will push the item into the queue using move semantic
   *        if move is true and noexcept in move.
   *
   * @param item The item to be pushed into queue
   * @param move True to move, else copy semantics
   */
  virtual void push(T &item, bool move = true);

  /**
   * @brief The method will put the client on blocking wait until
   *        the queue is empty, it returns number of items that
   *        were passed through the queue in total upon returning.
   *
   * @return The number of items that were passed through the queue
   *         in total
   */
  virtual long long waitForEmpty();

protected:
  /**
   * @brief The method will pop front item from the queue and return it
   *        or block waiting for front item if the queue is empty and
   *        wait is true.
   *
   * @param wait The caller is blocked waiting for item if queue is empty
   *             and wait is true, otherwise returning std::nullopt
   *
   * @return optional value from front item of the queue
   */
  virtual std::optional<T> popOptional(bool wait);

private:
  std::deque<T> m_queue{};
  pthread_mutex_t m_mutex{};
  pthread_cond_t m_cond{};
  pthread_cond_t m_empty_cond{};
  long long m_push_count{};
  long long m_pop_count{};
}; // class Dmn_Buffer

template <typename T> Dmn_Buffer<T>::Dmn_Buffer() {
  int err{};

  err = pthread_mutex_init(&m_mutex, NULL);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  err = pthread_cond_init(&m_cond, NULL);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  err = pthread_cond_init(&m_empty_cond, NULL);
  if (err) {
    throw std::runtime_error(strerror(err));
  }
}

template <typename T> Dmn_Buffer<T>::~Dmn_Buffer() noexcept try {
  // RAII! we wake up any thread waiting
  pthread_cond_signal(&m_cond);
  pthread_cond_signal(&m_empty_cond);

  pthread_cond_destroy(&m_empty_cond);
  pthread_cond_destroy(&m_cond);
  pthread_mutex_destroy(&m_mutex);
} catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

template <typename T> T Dmn_Buffer<T>::pop() { return *popOptional(true); }

template <typename T> std::optional<T> Dmn_Buffer<T>::popNoWait() {
  return popOptional(false);
}

template <typename T> void Dmn_Buffer<T>::push(T &&item) {
  T moved_item = std::move_if_noexcept(item);

  push(moved_item, true);
}

template <typename T> void Dmn_Buffer<T>::push(T &item, bool move) {
  int err{};

  err = pthread_mutex_lock(&m_mutex);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  DMN_PROC_ENTER_PTHREAD_MUTEX_CLEANUP(&m_mutex);

  pthread_testcancel();

  if (move) {
    m_queue.push_back(std::move_if_noexcept(item));
  } else {
    m_queue.push_back(item);
  }

  ++m_push_count;

  err = pthread_cond_signal(&m_cond);
  if (err) {
    pthread_mutex_unlock(&m_mutex);

    throw std::runtime_error(strerror(err));
  }

  DMN_PROC_EXIT_PTHREAD_MUTEX_CLEANUP();

  err = pthread_mutex_unlock(&m_mutex);
  if (err) {
    throw std::runtime_error(strerror(err));
  }
}

template <typename T> long long Dmn_Buffer<T>::waitForEmpty() {
  int err{};
  long long inbound_count{};

  err = pthread_mutex_lock(&m_mutex);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  DMN_PROC_ENTER_PTHREAD_MUTEX_CLEANUP(&m_mutex);

  pthread_testcancel();

  while (!m_queue.empty()) {
    err = pthread_cond_wait(&m_empty_cond, &m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();
  }

  assert(m_pop_count == m_push_count);
  inbound_count = m_pop_count;

  DMN_PROC_EXIT_PTHREAD_MUTEX_CLEANUP();

  err = pthread_mutex_unlock(&m_mutex);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  return inbound_count;
}

template <typename T> std::optional<T> Dmn_Buffer<T>::popOptional(bool wait) {
  int err{};
  T val{};

  err = pthread_mutex_lock(&m_mutex);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  DMN_PROC_ENTER_PTHREAD_MUTEX_CLEANUP(&m_mutex);

  pthread_testcancel();

  if (m_queue.empty()) {
    if (!wait) {
      err = pthread_mutex_unlock(&m_mutex);
      if (err) {
        throw std::runtime_error(strerror(err));
      }

      return {};
    }

    do {
      err = pthread_cond_wait(&m_cond, &m_mutex);
      if (err) {
        throw std::runtime_error(strerror(err));
      }

      pthread_testcancel();
    } while (m_queue.empty());
  }

  val = std::move(m_queue.front());
  m_queue.pop_front();

  ++m_pop_count;

  if (m_queue.empty()) {
    err = pthread_cond_signal(&m_empty_cond);
    if (err) {
      pthread_mutex_unlock(&m_mutex);

      throw std::runtime_error(strerror(err));
    }
  }

  DMN_PROC_EXIT_PTHREAD_MUTEX_CLEANUP();

  err = pthread_mutex_unlock(&m_mutex);
  if (err) {
    throw std::runtime_error(strerror(err));
  }

  return std::move_if_noexcept(val);
} // method popOptional()

} // namespace dmn

#endif // DMN_BUFFER_HPP_
