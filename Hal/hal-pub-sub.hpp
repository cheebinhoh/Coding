#ifndef HAL_PUB_SUB_HPP_HAVE_SEEN

#define HAL_PUB_SUB_HPP_HAVE_SEEN

#include "hal-async.hpp"

#include <functional>
#include <iostream>
#include <memory>
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

  virtual ~Hal_Pub() noexcept try {
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  void publish(T item) {
    // std::function<void()> functor {[this, item](){
    // this->publishInternal(item); }};

    //  this->write(functor);
    HAL_ASYNC_CALL_WITH_CAPTURE({ this->publishInternal(item); }, this, item);
  }

  void registerSubscriber(Hal_Sub *sub) {
    HAL_ASYNC_CALL_WITH_CAPTURE((this->registerSubscriberInternal(sub)), this,
                                sub);
  }

protected:
  void publishInternal(T item) {
    for (auto &sub : m_subscribers) {
      sub->notifyInternal(item);
    }
  }

  void registerSubscriberInternal(Hal_Sub *sub) {
    m_subscribers.push_back(sub);
  }

private:
  std::vector<Hal_Sub *> m_subscribers{};
};

#endif /* HAL_PUB_SUB_HPP_HAVE_SEEN */
