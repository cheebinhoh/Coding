/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 */

#include "hal-singleton.hpp"

#include <gtest/gtest.h>

#include <iostream>

class Hal_A : public Hal::Hal_Singleton {
public:
  Hal_A(int int1, int int2) : m_int1{int1}, m_int2{int2} {}

  int getValue() { return m_int1 + m_int2; }

  template <class... U>
  static std::shared_ptr<Hal_A> createInstanceInternal(U &&...u) {
    if (!Hal_A::s_instances) {
      std::call_once(
          s_InitOnce,
          [](U &&...arg) {
            Hal_A::s_instances =
                std::make_shared<Hal_A>(std::forward<U>(arg)...);
          },
          std::forward<U>(u)...);
    }

    return Hal_A::s_instances;
  }

private:
  static std::shared_ptr<Hal_A> s_instances;
  static std::once_flag s_InitOnce;

  int m_int1{};
  int m_int2{};
};

std::once_flag Hal_A::s_InitOnce{};
std::shared_ptr<Hal_A> Hal_A::s_instances{};

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  auto inst1 = Hal_A::createInstance<Hal_A>(1, 2);
  std::cout << "Value: " << inst1->getValue() << ", :" << inst1 << "\n";

  auto inst2 = Hal_A::createInstance<Hal_A>(1, 2);
  std::cout << "Value: " << inst2->getValue() << ", :" << inst2 << "\n";

  EXPECT_TRUE(inst1 == inst2);

  return RUN_ALL_TESTS();
}

