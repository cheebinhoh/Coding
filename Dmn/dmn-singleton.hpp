/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements a singleton class that subclass can inheritted
 * from and used to create a singleton instance with arguments specific
 * to the subclass constructor.
 */

#ifndef DMN_SINGLETON_HPP_HAVE_SEEN

#define DMN_SINGLETON_HPP_HAVE_SEEN

#include <memory>
#include <utility>

namespace Dmn {

class Dmn_Singleton {
public:
  template <typename T, class... U>
  static std::shared_ptr<T> createInstance(U &&...u) {
    std::shared_ptr<T> new_instance =
        T::createInstanceInternal(std::forward<U>(u)...);

    return new_instance;
  }
};

} // namespace Dmn

#endif /* DMN_SINGLETON_HPP_HAVE_SEEN */
