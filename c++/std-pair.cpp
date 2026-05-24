/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program demonstrates std::pair, a special case
 * of std::tuple.
 */

#include <iostream>
#include <string>
#include <utility>

int main(int argc, char *argv[]) {
  std::pair<int, std::string> one = {1, "one"};
  std::pair<int, std::string> two = std::make_pair(2, "two");
  auto three = std::make_pair<int, std::string>(3, "three");

  std::cout << "pair of values: (first = " << one.first << ", second = \""
            << one.second << "\")\n";

  std::cout << "pair of values: (first = " << two.first << ", second = \""
            << two.second << "\")\n";

  std::cout << "pair of values: (first = " << three.first << ", second = \""
            << three.second << "\")\n";

  std::cout << "\n";

  return 0;
}
