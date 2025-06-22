/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-singleton.hpp
 * @brief The header file for dmn-singleton which implements a singleton class
 *        that subclass can inherit from and used to create a singleton instance
 *        with arguments specific to the subclass constructor.
 */

#ifndef DMN_SINGLETON_HPP_

#define DMN_SINGLETON_HPP_

#include <memory>
#include <utility>

namespace dmn {

class Dmn_Singleton {
public:
  /**
   * @brief The static method called to create singleton instance of the
   *        template type, and the concrete type specific singleton instance
   *        creation is done by template type (T) which implements static
   *        createInstanceInternal() that creates and always return same
   *        instance.
   *
   * @param arg The varying forwarded arguments that depends on the template
   *            type' createInstanceInternal() method arguments
   */
  template <typename T, class... U>
  static std::shared_ptr<T> createInstance(U &&...arg);
};

template <typename T, class... U>
std::shared_ptr<T> Dmn_Singleton::createInstance(U &&...arg) {
  // NOTE that the template type class is supposed to handle
  // thread-safety (example through std::once_flag) to make
  // sure that multiple threads calling createInstance is
  // thread safe and always the singleton instance is returned.

  std::shared_ptr<T> new_instance =
      T::createInstanceInternal(std::forward<U>(arg)...);

  return new_instance;
}

} // namespace dmn

#endif // DMN_SINGLETON_HPP_
