/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements a fifo buffer that:
 * - push is not blocking
 * - pop is blocking (optional) if no data in the buffer
 */

#ifndef HAL_BUFFER_HPP_HAVE_SEEN

#define HAL_BUFFER_HPP_HAVE_SEEN

#include <algorithm>
#include <cassert>
#include <cstring>
#include <deque>

#include <pthread.h>

template <typename T> class Hal_Buffer {
public:
  Hal_Buffer() {
    int err{};

    err = pthread_mutex_init(&m_mutex, NULL);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    err = pthread_cond_init(&m_cond, NULL);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    err = pthread_cond_init(&m_emptyCond, NULL);
    if (err) {
      throw std::runtime_error(strerror(err));
    }
  }

  virtual ~Hal_Buffer() {
    // RAII! we wake up any thread waiting for the buffer in pop method!
    pthread_cond_signal(&m_cond);

    pthread_cond_destroy(&m_emptyCond);
    pthread_cond_destroy(&m_cond);
    pthread_mutex_destroy(&m_mutex);
  }

  Hal_Buffer(const Hal_Buffer<T> &halBuffer) = delete;
  const Hal_Buffer<T> &operator=(const Hal_Buffer<T> &halBuffer) = delete;
  Hal_Buffer(const Hal_Buffer<T> &&halBuffer) = delete;
  Hal_Buffer<T> &operator=(Hal_Buffer<T> &&halBuffer) = delete;

  /**
   * @brief The method will pop and return front item from the queue or the
   *        caller is blocked waiting if the queue is empty
   *
   * @return front item of the queue.
   */
  virtual T pop() { return *popOptional(true); }

  /**
   * @brief The method will pop and return front item from the queue or the
   *        std::nullopt if the queue is empty.
   *
   * @return optional item from the front of the queue
   */
  virtual std::optional<T> popNoWait() { return popOptional(false); }

  /**
   * @brief The method will push the item into buffer using move semantics
   *        unless noexcept is false.
   *
   * @param rItem The item to be pushed into buffer
   */
  virtual void push(T &&rItem) {
    T movedItem = std::move_if_noexcept(rItem);

    push(movedItem, true);
  }

  /**
   * @brief The method will push the item into the buffer using move semantic
   *        if move is true and noexcept in move.
   *
   * @param rItem The item to be pushed into buffer
   * @param move  True to move, else copy semantics
   */
  virtual void push(T &rItem, bool move = true) {
    int err{};

    err = pthread_mutex_lock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    if (move) {
      m_queue.push_back(std::move_if_noexcept(rItem));
    } else {
      m_queue.push_back(rItem);
    }

    ++m_pushCount;

    err = pthread_cond_signal(&m_cond);
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
   * @brief The method will put the client on blocking wait until
   *        the queue is emptied, it returns number of items that
   *        were passed through the queue in total.
   *
   * @return The number of items that were passed through the queue
   *         in total
   */
  virtual long long waitForEmpty() {
    int err{};
    long long inboundCount{};

    err = pthread_mutex_lock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    while (!m_queue.empty()) {
      err = pthread_cond_wait(&m_emptyCond, &m_mutex);
      if (err) {
        throw std::runtime_error(strerror(err));
      }

      pthread_testcancel();
    }

    assert(m_popCount == m_pushCount);
    inboundCount = m_popCount;

    err = pthread_mutex_unlock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    return inboundCount;
  }

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
  virtual std::optional<T> popOptional(bool wait) {
    int err{};

    err = pthread_mutex_lock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

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

    T val = std::move(m_queue.front());
    m_queue.pop_front();

    ++m_popCount;

    err = pthread_cond_signal(&m_emptyCond);
    if (err) {
      pthread_mutex_unlock(&m_mutex);

      throw std::runtime_error(strerror(err));
    }

    err = pthread_mutex_unlock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    return std::move_if_noexcept(val);
  }

private:
  std::deque<T> m_queue{};
  pthread_mutex_t m_mutex{};
  pthread_cond_t m_cond{};
  pthread_cond_t m_emptyCond{};
  long long m_pushCount{};
  long long m_popCount{};
};

#endif /* HAL_BUFFER_HPP_HAVE_SEEN */
