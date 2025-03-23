/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements an fifo queue that:
 * - push is not blocking
 * - pop is blocking (optional) if no data in the queue
 *
 * There is no programmatic limit on the size of the queue,
 * but runtime memory limit or size type of the underlying
 * STL class it uses.
 */

#ifndef DMN_BUFFER_HPP_HAVE_SEEN

#define DMN_BUFFER_HPP_HAVE_SEEN

#include <algorithm>
#include <cassert>
#include <cstring>
#include <deque>
#include <stdexcept>

#include <pthread.h>

namespace Dmn {

template <typename T> class Dmn_Buffer {
public:
  Dmn_Buffer() {
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

  virtual ~Dmn_Buffer() noexcept try {
    // RAII! we wake up any thread waiting
    pthread_cond_signal(&m_cond);
    pthread_cond_signal(&m_emptyCond);

    pthread_cond_destroy(&m_emptyCond);
    pthread_cond_destroy(&m_cond);
    pthread_mutex_destroy(&m_mutex);
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  Dmn_Buffer(const Dmn_Buffer<T> &dmnBuffer) = delete;
  const Dmn_Buffer<T> &operator=(const Dmn_Buffer<T> &dmnBuffer) = delete;
  Dmn_Buffer(const Dmn_Buffer<T> &&dmnBuffer) = delete;
  Dmn_Buffer<T> &operator=(Dmn_Buffer<T> &&dmnBuffer) = delete;

  /**
   * @brief The method will pop and return front item from the queue or the
   *        caller is blocked waiting if the queue is empty.
   *
   * @return front item of the queue
   */
  virtual T pop() { return *popOptional(true); }

  /**
   * @brief The method will pop and return front item from the queue or the
   *        std::nullopt if the queue is empty (and caller is not blocked
   *        waiting).
   *
   * @return optional item from the front of the queue
   */
  virtual std::optional<T> popNoWait() { return popOptional(false); }

  /**
   * @brief The method will push the item into queue using move semantics
   *        unless noexcept is false.
   *
   * @param item The item to be pushed into queue
   */
  virtual void push(T &&item) {
    T movedItem = std::move_if_noexcept(item);

    push(movedItem, true);
  }

  /**
   * @brief The method will push the item into the queue using move semantic
   *        if move is true and noexcept in move.
   *
   * @param item The item to be pushed into queue
   * @param move  True to move, else copy semantics
   */
  virtual void push(T &item, bool move = true) {
    int err{};

    err = pthread_mutex_lock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    if (move) {
      m_queue.push_back(std::move_if_noexcept(item));
    } else {
      m_queue.push_back(item);
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
   *        the queue is empty, it returns number of items that
   *        were passed through the queue in total upon returning.
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

    if (m_queue.empty()) {
      err = pthread_cond_signal(&m_emptyCond);
      if (err) {
        pthread_mutex_unlock(&m_mutex);

        throw std::runtime_error(strerror(err));
      }
    }

    err = pthread_mutex_unlock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    return std::move_if_noexcept(val);
  } /* End of method popOptional() */

private:
  std::deque<T> m_queue{};
  pthread_mutex_t m_mutex{};
  pthread_cond_t m_cond{};
  pthread_cond_t m_emptyCond{};
  long long m_pushCount{};
  long long m_popCount{};
}; /* End of class Dmn_Buffer */

} /* End of namespace Dmn */

#endif /* End of macro DMN_BUFFER_HPP_HAVE_SEEN */
