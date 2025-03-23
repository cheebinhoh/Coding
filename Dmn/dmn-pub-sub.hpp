/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements a simple publisher subscriber model, the class
 * templatize the data to be published and subscribed of.
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
   * Subscriber will be notified of published data item via Dmn_Pub
   * (publisher).
   *
   * Subscriber subclass can override notify() method to have specific
   * functionality.
   */
  class Dmn_Sub : public Dmn_Async {
  public:
    Dmn_Sub() = default;

    virtual ~Dmn_Sub() noexcept try {
      if (m_pub) {
        m_pub->unregisterSubscriber(this);
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
     *        publisher, it is called within the subscriber's own singleton
     *        asynchronous thread context.
     *
     * @param item The data item notified by publisher
     */
    virtual void notify(T item) = 0;

    friend class Dmn_Pub;

  private:
    /**
     * @brief The method is supposed to be called by publisher (the friend
     *        class) to notify the subscriber of the data item published
     *        by publisher in the subscriber' singleton asynchronous thread
     *        context.
     *
     * @param item The data item notified by publisher
     */
    void notifyInternal(T item) {
      DMN_ASYNC_CALL_WITH_CAPTURE({ this->notify(item); }, this, item);
    }

    Dmn_Pub *m_pub{};
  }; // End of class Dmn_Sub

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
      sub->m_pub = nullptr;
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
   *        Publisher's singleton asynchronous thread context, this allows the
   *        method execution & caller remains low-latency regardless of the
   *        number of subscribers and amount of publishing data item.
   *
   * @param item The data item to be published to subscribers
   */
  void publish(T item) {
    DMN_ASYNC_CALL_WITH_CAPTURE({ this->publishInternal(item); }, this, item);
  }

  /**
   * @brief The method registers a subscriber and send out prior published data
   *        item, The method is called immediately with m_subscribersLock
   *        than executed in the singleton asynchronous context, and that
   *        allows caller to be sure that the Dmn_Sub instance has been
   *        registered with the publisher upon return of the method. The
   *        register and unregister methods work in synchronization manner.
   *
   * @param sub The subscriber instance to be registered
   */
  void registerSubscriber(Dmn_Sub *sub) {
    if (this == sub->m_pub) {
      return;
    }

    assert(nullptr == sub->m_pub ||
           "The subscriber has been registered with another publisher");

    std::lock_guard<std::mutex> lck(m_subscribersLock);
    m_subscribers.push_back(sub);
    sub->m_pub = this;

    // resend the data items that the registered subscriber
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
   *        immediately with m_subscribersLock than executed in the
   *        singleton asynchronous thread context, and that allows caller to
   *        be sure that the Dmn_Sub instance has been de-registered
   *        from the publisher upon return of the method.
   *
   * @param sub The subscriber instance to be de-registered
   */
  void unregisterSubscriber(Dmn_Sub *sub) {
    if (nullptr == sub->m_pub) {
      return;
    }

    assert(this == sub->m_pub ||
           "The subscriber' registered publisher is NOT this" == nullptr);

    std::lock_guard<std::mutex> lck(m_subscribersLock);
    sub->m_pub = nullptr;
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
    /* Though through Dmn_Async (parent class), we have a mean to
     * guarantee that only one thread is executing the core logic
     * and manipulate the m_subscribers state in a singleton asynchronous
     * thread context and without the use of mutex to protect the m_subscribers,
     * but that also requires both registerSubscriber() and
     * unregisterSubscriber() API methods to have it side effects executed
     * in the singleton asynchronous thread context, and which means that upon
     * returning from both API methods, the client is not guaranteed that
     * it has registered or unregistered successfully.
     *
     * A simple mutex will guarantee that and the penalty of mutex is small
     * as most clients are registered upon application brought up, and only
     * deregister upon application shutdown.
     */
    std::lock_guard<std::mutex> lck(m_subscribersLock);

    /* Keep the published item in circular ring buffer for
     * efficient access to playback to new subscribers whose misses the
     * data.
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
  } /* End of method publishInternal */

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

  /**
   * data members for constructor to instantiate the object.
   */
  std::string m_name{};
  ssize_t m_capacity{};
  Dmn_Pub_Filter_Task m_filterFn{};

  /**
   * data members for internal logic.
   */
  std::vector<T> m_buffer{};
  ssize_t m_first{};
  ssize_t m_next{};

  std::mutex m_subscribersLock{};
  std::vector<Dmn_Sub *> m_subscribers{};
}; /* End of class Dmn_Pub */

} /* End of namespace Dmn */

#endif /* End of macro DMN_PUB_SUB_HPP_HAVE_SEEN */
