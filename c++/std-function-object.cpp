/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program demonstrates std::functional and function object.
 */

#include <functional>
#include <iostream>

class AdditionInt {
public:
  int operator()(int num1, int num2) { return num1 + num2; }
};

int add_fn(int num1, int num2) { return num1 + num2; }

int call_addition(std::function<int(int, int)> f, int num1, int num2) {
  return f(num1, num2);
}

int main(int argc, char *argv[]) {
  class AdditionInt addobj {};

  std::cout << "add two number via function = " << call_addition(add_fn, 1, 2)
            << "\n";
  std::cout << "add two number via function = " << call_addition(addobj, 1, 2)
            << "\n";
  std::cout << "add two number via function = "
            << call_addition(std::plus<int>(), 1, 2) << "\n";
  std::cout << "add two number via function = " << addobj(1, 2) << "\n";

  return 0;
}
