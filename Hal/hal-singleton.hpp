
#ifndef HAL_SINGLETON_HPP_HAVE_SEEN

#define HAL_SINGLETON_HPP_HAVE_SEEN

#include <memory>
#include <mutex>
#include <utility>

#include <iostream>

class Hal_Singleton {
public:
  template <typename T, class... U>
  static std::shared_ptr<T> createInstance(U &&...u) {
    std::shared_ptr<T> new_instance =
        T::createInstanceInternal(std::forward<U>(u)...);

    return new_instance;
  }
};

#endif /* HAL_SINGLETON_HPP_HAVE_SEEN */
