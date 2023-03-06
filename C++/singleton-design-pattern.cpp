/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 */

#include <iostream>
#include <memory>

class Singleton {
private:
  static std::unique_ptr<Singleton> m_instance;
};

std::unique_ptr<Singleton> Singleton::m_instance{};

int main(int argc, char *argv[]) { return 0; }
