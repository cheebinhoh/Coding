/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements a fifo queue with a limited capacity that:
 * - push is blocking if the limit of the capacity is reach
 * - pop is blocking if no data (optional) in the queue
 */

#ifndef HAL_LIMITBUFFER_HPP_HAVE_SEEN

#define HAL_LIMITBUFFER_HPP_HAVE_SEEN

#include "hal-buffer.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <deque>
#include <optional>
#include <stdexcept>

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
    pthread_cond_signal(&m_popCond);
    pthread_cond_signal(&m_pushCond);

    pthread_cond_destroy(&m_pushCond);
    pthread_cond_destroy(&m_popCond);
    pthread_mutex_destroy(&m_mutex);
  }

  Hal_LimitBuffer(const Hal_LimitBuffer<T> &halLimitBuffer) = delete;
  const Hal_LimitBuffer<T> &
  operator=(const Hal_LimitBuffer<T> &halLimitBuffer) = delete;
  Hal_LimitBuffer(Hal_LimitBuffer<T> &&halLimitBuffer) = delete;
  Hal_LimitBuffer<T> &&operator=(Hal_LimitBuffer<T> &&halLimitBuffer) = delete;

  /**
   * @brief The method will pop and return front item from the queue or the
   *        caller is blocked waiting if the queue is empty.
   *
   * @return front item of the queue
   */
  T pop() override { return *popOptional(true); }

  /**
   * @brief The method will pop and return front item from the queue or the
   *        std::nullopt if the queue is empty.
   *
   * @return optional item from the front of the queue
   */
  std::optional<T> popNoWait() override { return popOptional(false); }

  /**
   * @brief The method will push the item into queue using move semantics
   *        unless noexcept is false. The caller is blocked waiting if the
   *        queue is full.
   *
   * @param item The item to be pushed into queue
   */
  void push(T &&item) override {
    T movedItem = std::move_if_noexcept(item);

    push(movedItem, true);
  }

  /**
   * @brief The method will push the item into queue using move semantics
   *        if move is true (and noexcept is true). The caller is blocked
   *        waiting if the queue is full.
   *
   * @param item The item to be pushed into queue
   * @param move True to use move semantics, else copy semantic
   */
  void push(T &item, bool move = true) override {
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

    Hal_Buffer<T>::push(item, move);
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

  /**
   * @brief The method returns the number of items held in the queue now.
   *
   * @return The number of items held in the queue now
   */
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

  /**
   * @brief The method will put the client on blocking wait until
   *        the queue is empty, it returns number of items that
   *        were passed through the queue in total.
   *
   * @return The number of items that were passed through the queue
   *         in total
   */
  long long waitForEmpty() override { return Hal_Buffer<T>::waitForEmpty(); }

private:
  /**
   * @brief The method will pop front item from the queue and return it
   *        or block waiting for item if the queue is empty and wait is
   *        true.
   *
   * @param wait The caller is blocked waiting for item if queue is empty
   *             and wait is true, otherwise returning std::nullopt
   *
   * @return optional value from front item of the queue
   */
  std::optional<T> popOptional(bool wait) override {
    int err{};

    err = pthread_mutex_lock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    auto val = Hal_Buffer<T>::popOptional(wait);
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
