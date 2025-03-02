/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements a simple form of publisher subscriber model
 */

#ifndef HAL_PUB_SUB_HPP_HAVE_SEEN

#define HAL_PUB_SUB_HPP_HAVE_SEEN

#include "hal-async.hpp"

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

namespace Hal {

template <typename T> class Hal_Pub : public Hal_Async {
public:
  /**
   * The Subscriber class implements the Interface of Subscriber to be
   * working with Publisher.
   *
   * Subscriber should inherit from the Hal_Sub to be notified of any
   * data item from Hal_Pub (publisher), the Hal_Sub interface methods are
   * called in the Hal_Sub own async thread context.
   *
   * Subscriber subclass can override notify() method to have specific
   * functionality.
   */
  class Hal_Sub : public Hal_Async {
  public:
    Hal_Sub() = default;

    virtual ~Hal_Sub() noexcept try {
      if (pub) {
        pub->unregisterSubscriber(this);
      }
    } catch (...) {
      // explicit return to resolve exception as destructor must be noexcept
      return;
    }

    Hal_Sub(const Hal_Sub &halSub) = delete;
    const Hal_Sub &operator=(const Hal_Sub &halSub) = delete;
    Hal_Sub(Hal_Sub &&halSub) = delete;
    Hal_Sub &operator=(Hal_Sub &&halSub) = delete;

    /**
     * @brief The method notifies the subscriber of the data item from
     * publisher, it is called within the subscriber's own async thread context.
     *
     * @param item The data item notified by publisher
     */
    virtual void notify(T item) = 0;

    friend class Hal_Pub;

  private:
    /**
     * @brief The method is supposed to be called by publisher (a friend class)
     *        to async notify the subscriber of the item published by publisher.
     *
     * @param item The data item notified by publisher
     */
    void notifyInternal(T item) {
      HAL_ASYNC_CALL_WITH_CAPTURE({ this->notify(item); }, this, item);
    }

    Hal_Pub *pub{};
  };
  // End of class Hal_Sub

  using Hal_Pub_Filter_Task =
      std::function<bool(const Hal_Sub *const, const T &t)>;

  Hal_Pub(std::string_view name, ssize_t capacity = 10,
          Hal_Pub_Filter_Task filterFn = {})
      : Hal_Async(name), m_name{name}, m_capacity{capacity},
        m_filterFn{filterFn} {
    resize(capacity);
  }

  virtual ~Hal_Pub() noexcept try {
    std::lock_guard<std::mutex> lck(m_subscribersLock);

    for (auto &sub : m_subscribers) {
      sub->pub = nullptr;
    }
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  Hal_Pub(const Hal_Pub &halPub) = delete;
  const Hal_Pub &operator=(const Hal_Pub &halPub) = delete;
  Hal_Pub(Hal_Pub &&halPub) = delete;
  Hal_Pub &operator=(Hal_Pub &&halPub) = delete;

  /**
   * @brief The method copies the item and publish it to all subscribers. The
   *        method does not publish message directly but execute it in
   *        Publisher's async thread context, this allows the method & caller
   *        remains low-latency regardless of the number of subscribers and
   *        amount of publishing data item.
   *
   * @param item The data item to be published to subscribers
   */
  void publish(T item) {
    HAL_ASYNC_CALL_WITH_CAPTURE({ this->publishInternal(item); }, this, item);
  }

  /**
   * @brief The method registers a subscriber and send out backdate data
   *        item, The method is called immediately with m_subscribersLock
   *        than executed in async thread, and that allows caller to be
   *        sure that the Hal_Sub instance has been registered with the
   *        publisher upon return of the method. The register and unregister
   *        methods work in synchronization manner.
   *
   * @param sub The subscriber instance to be registered
   */
  void registerSubscriber(Hal_Sub *sub) {
    std::lock_guard<std::mutex> lck(m_subscribersLock);
    m_subscribers.push_back(sub);
    sub->pub = this;

    // resend backtrace data items that the registered subscriber
    // miss.
    if (m_next > m_first) {
      for (std::size_t n = m_first; n < m_next; n++) {
        sub->notifyInternal(m_buffer[n]);
      }
    } else if (m_first > 0) {
      for (std::size_t n = m_first; n < m_capacity; n++) {
        sub->notifyInternal(m_buffer[n]);
      }

      for (std::size_t n = 0; n < m_first; n++) {
        sub->notifyInternal(m_buffer[n]);
      }
    }
  }

  /**
   * @brief The method de-registers a subscriber. The method is called
   *        immediately with m_subscribersLock than executed in async
   *        thread, and that allows caller to be sure that the Hal_Sub
   *        instance has been de-registered from the publisher upon
   *        return of the method.
   *
   * @param sub The subscriber instance to be de-registered
   */
  void unregisterSubscriber(Hal_Sub *sub) {
    std::lock_guard<std::mutex> lck(m_subscribersLock);
    sub->pub = nullptr;
    m_subscribers.erase(
        std::remove(m_subscribers.begin(), m_subscribers.end(), sub),
        m_subscribers.end());
  }

protected:
  /**
   * @brief The method publishes data item to registered subscribers.
   *
   * @param item The data item to be published
   */
  virtual void publishInternal(T item) {
    /* Though the Hal_Pipe async thread, we are guarantee that only
     * one thread is executing the Internal method but unregister
     * has to be run in caller thread and changes the core data
     * upon return, so we need to have an extra m_subscribersLock
     */
    std::lock_guard<std::mutex> lck(m_subscribersLock);

    /* Keep the published item in circular ring buffer for
     * efficient access to playback!
     */
    if (m_next >= m_capacity) {
      m_next = 0;
      m_first = (m_first + 1) >= m_capacity ? 0 : m_first + 1;
    } else if (m_first > 0 && m_next >= m_first) {
      m_first = (m_first + 1) >= m_capacity ? 0 : m_first + 1;
    }

    m_buffer[m_next] = item;
    m_next++;

    for (auto &sub : m_subscribers) {
      if (!m_filterFn || m_filterFn(sub, item)) {
        sub->notifyInternal(item);
      }
    }
  }

private:
  /**
   * @brief The method will resize the capacity of the publisher's buffers.
   *
   * @param size The new capacity size for the publisher's buffer
   */
  void resize(ssize_t size) {
    m_capacity = size;
    m_buffer.reserve(m_capacity);
  }

  std::string m_name{};

  ssize_t m_capacity{};
  Hal_Pub_Filter_Task m_filterFn{};
  std::vector<T> m_buffer{};
  ssize_t m_first{};
  ssize_t m_next{};

  std::mutex m_subscribersLock{};
  std::vector<Hal_Sub *> m_subscribers{};
};

} // namespace Hal

#endif /* HAL_PUB_SUB_HPP_HAVE_SEEN */
