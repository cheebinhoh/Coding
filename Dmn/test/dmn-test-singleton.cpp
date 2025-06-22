/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include <gtest/gtest.h>

#include <iostream>
#include <thread>

#include "dmn-singleton.hpp"

class Dmn_A : public dmn::Dmn_Singleton {
public:
  Dmn_A(int int1, int int2) : m_int1{int1}, m_int2{int2} {}

  int getValue() { return m_int1 + m_int2; }

  template <class... U>
  static std::shared_ptr<Dmn_A> createInstanceInternal(U &&...u) {
    if (!Dmn_A::s_instances) {
      std::call_once(
          s_init_once,
          [](U &&...arg) {
            Dmn_A::s_instances =
                std::make_shared<Dmn_A>(std::forward<U>(arg)...);
          },
          std::forward<U>(u)...);
    }

    return Dmn_A::s_instances;
  }

private:
  static std::shared_ptr<Dmn_A> s_instances;
  static std::once_flag s_init_once;

  int m_int1{};
  int m_int2{};
};

std::once_flag Dmn_A::s_init_once{};
std::shared_ptr<Dmn_A> Dmn_A::s_instances{};

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  auto inst1 = Dmn_A::createInstance<Dmn_A>(1, 2);
  std::cout << "Value: " << inst1->getValue() << ", :" << inst1 << "\n";

  auto inst2 = Dmn_A::createInstance<Dmn_A>(1, 2);
  std::cout << "Value: " << inst2->getValue() << ", :" << inst2 << "\n";

  EXPECT_TRUE(inst1 == inst2);

  return RUN_ALL_TESTS();
}
