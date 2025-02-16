/**
 * This class implements a limited form of publisher subscriber model
 */
#ifndef HAL_PUB_SUB_HPP_HAVE_SEEN

#define HAL_PUB_SUB_HPP_HAVE_SEEN

#include "hal-async.hpp"

#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

template <typename T> class Hal_Pub : public Hal_Async {
public:
  class Hal_Sub : public Hal_Async {
  public:
    virtual ~Hal_Sub() noexcept try {
    } catch (...) {
      // explicit return to resolve exception as destructor must be noexcept
      return;
    }

    virtual void notify(T item) = 0;

    friend class Hal_Pub;

  private:
    void notifyInternal(T item) {
      HAL_ASYNC_CALL_WITH_CAPTURE({ this->notify(item); }, this, item);
    }
  };

  Hal_Pub(std::string_view name, ssize_t capacity = 2)
      : Hal_Async(name), m_name{name}, m_capacity{capacity} {
    resize(capacity);
  }

  virtual ~Hal_Pub() noexcept try {
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  /**
   * @brief The method copy the item and publish it to all subscribers!
   */
  void publish(T item) {
    HAL_ASYNC_CALL_WITH_CAPTURE({ this->publishInternal(item); }, this, item);
  }

  void registerSubscriber(Hal_Sub *sub) {
    HAL_ASYNC_CALL_WITH_CAPTURE((this->registerSubscriberInternal(sub)), this,
                                sub);
  }

protected:
  void publishInternal(T item) {
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
      sub->notifyInternal(item);
    }
  }

  void registerSubscriberInternal(Hal_Sub *sub) {
    m_subscribers.push_back(sub);

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
  std::vector<T> m_buffer{};
  std::vector<Hal_Sub *> m_subscribers{};
  std::size_t m_first{};
  std::size_t m_next{};
};

#endif /* HAL_PUB_SUB_HPP_HAVE_SEEN */
