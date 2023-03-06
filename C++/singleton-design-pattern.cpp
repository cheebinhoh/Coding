/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program demonstrates the class creational design pattern, Singleton.
 *
 * Note that we make use of smart pointer std::unique_ptr as a placeholder to
 * to release the Singleton object that is defined in global scope where
 * destructor is not called in default if the instance is dynamically allocated
 * (via new) as we do not provide a delete (at_exit will be the good place to do
 * so, but it assumes certain platform.
 */

#include <iostream>
#include <memory>

class Singleton {
public:
  static Singleton *getInstance() {

    if (!m_instance) {
      std::cout << "No instance, create one\n";

      std::unique_ptr<Singleton> new_instance{new Singleton};
      m_instance = std::move(new_instance);
    } else {
      std::cout << "Instance exist, return it\n";
    }

    return m_instance.get();
  }

  ~Singleton() { std::cout << "Deinitialize Singleton object\n"; }

protected:
  Singleton() {}

private:
  static std::unique_ptr<Singleton> m_instance;
};

std::unique_ptr<Singleton> Singleton::m_instance{};

int main(int argc, char *argv[]) {
  std::cout << "Get singleton instance\n";
  Singleton *inst = Singleton::getInstance();

  {
    std::cout << "Get singleton instance again\n";
    Singleton *nested_inst = Singleton::getInstance();

    std::cout << "Is same instance = " << std::boolalpha
              << (nested_inst == inst) << "\n";
  }

  return 0;
}
