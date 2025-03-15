/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements a simple form of publisher subscriber model
 */

#ifndef DMN_PUB_SUB_HPP_HAVE_SEEN

#define DMN_PUB_SUB_HPP_HAVE_SEEN

#include "dmn-async.hpp"

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

namespace Dmn {

template <typename T> class Dmn_Pub : public Dmn_Async {
public:
  /**
   * The Subscriber class implements the Interface of Subscriber to be
   * working with Publisher.
   *
   * Subscriber should inherit from the Dmn_Sub to be notified of any
   * data item from Dmn_Pub (publisher), the Dmn_Sub interface methods are
   * called in the Dmn_Sub own async thread context.
   *
   * Subscriber subclass can override notify() method to have specific
   * functionality.
   */
  class Dmn_Sub : public Dmn_Async {
  public:
    Dmn_Sub() = default;

    virtual ~Dmn_Sub() noexcept try {
      if (pub) {
        pub->unregisterSubscriber(this);
      }
    } catch (...) {
      // explicit return to resolve exception as destructor must be noexcept
      return;
    }

    Dmn_Sub(const Dmn_Sub &dmnSub) = delete;
    const Dmn_Sub &operator=(const Dmn_Sub &dmnSub) = delete;
    Dmn_Sub(Dmn_Sub &&dmnSub) = delete;
    Dmn_Sub &operator=(Dmn_Sub &&dmnSub) = delete;

    /**
     * @brief The method notifies the subscriber of the data item from
     * publisher, it is called within the subscriber's own async thread context.
     *
     * @param item The data item notified by publisher
     */
    virtual void notify(T item) = 0;

    friend class Dmn_Pub;

  private:
    /**
     * @brief The method is supposed to be called by publisher (a friend class)
     *        to async notify the subscriber of the item published by publisher.
     *
     * @param item The data item notified by publisher
     */
    void notifyInternal(T item) {
      DMN_ASYNC_CALL_WITH_CAPTURE({ this->notify(item); }, this, item);
    }

    Dmn_Pub *pub{};
  };
  // End of class Dmn_Sub

  using Dmn_Pub_Filter_Task =
      std::function<bool(const Dmn_Sub *const, const T &t)>;

  Dmn_Pub(std::string_view name, ssize_t capacity = 10,
          Dmn_Pub_Filter_Task filterFn = {})
      : Dmn_Async(name), m_name{name}, m_capacity{capacity},
        m_filterFn{filterFn} {
    resize(capacity);
  }

  virtual ~Dmn_Pub() noexcept try {
    std::lock_guard<std::mutex> lck(m_subscribersLock);

    for (auto &sub : m_subscribers) {
      sub->pub = nullptr;
    }
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  Dmn_Pub(const Dmn_Pub &dmnPub) = delete;
  const Dmn_Pub &operator=(const Dmn_Pub &dmnPub) = delete;
  Dmn_Pub(Dmn_Pub &&dmnPub) = delete;
  Dmn_Pub &operator=(Dmn_Pub &&dmnPub) = delete;

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
    DMN_ASYNC_CALL_WITH_CAPTURE({ this->publishInternal(item); }, this, item);
  }

  /**
   * @brief The method registers a subscriber and send out backdate data
   *        item, The method is called immediately with m_subscribersLock
   *        than executed in async thread, and that allows caller to be
   *        sure that the Dmn_Sub instance has been registered with the
   *        publisher upon return of the method. The register and unregister
   *        methods work in synchronization manner.
   *
   * @param sub The subscriber instance to be registered
   */
  void registerSubscriber(Dmn_Sub *sub) {
    std::lock_guard<std::mutex> lck(m_subscribersLock);
    m_subscribers.push_back(sub);
    sub->pub = this;

    // resend backdate data items that the registered subscriber
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
   *        thread, and that allows caller to be sure that the Dmn_Sub
   *        instance has been de-registered from the publisher upon
   *        return of the method.
   *
   * @param sub The subscriber instance to be de-registered
   */
  void unregisterSubscriber(Dmn_Sub *sub) {
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
    /* Though the Dmn_Pipe async thread, we are guarantee that only
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
  Dmn_Pub_Filter_Task m_filterFn{};

  std::vector<T> m_buffer{};
  ssize_t m_first{};
  ssize_t m_next{};

  std::mutex m_subscribersLock{};
  std::vector<Dmn_Sub *> m_subscribers{};
};

} // namespace Dmn

#endif /* DMN_PUB_SUB_HPP_HAVE_SEEN */
