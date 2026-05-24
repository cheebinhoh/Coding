/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program demonstrates tuple.
 */

#include <iostream>
#include <string>
#include <tuple>

int main(int argc, char *argv[]) {
  std::tuple<int, int, std::string> one_two{1, 2, "two"};
  auto two_two = std::make_tuple(2, 2, "four");
  auto three_two = std::make_tuple(3, 2, "six");
  auto four_two = std::make_tuple(4, 2, "eight");
  int first;
  int second;
  std::string answer;

  std::cout << std::get<0>(one_two) << " x " << std::get<1>(one_two) << " is "
            << std::get<2>(one_two) << "\n";

  std::cout << std::get<0>(two_two) << " x " << std::get<1>(two_two) << " is "
            << std::get<2>(two_two) << "\n";

  std::tie(first, second, answer) = three_two;
  std::cout << first << " x " << second << " is " << answer << "\n";

  std::tie(first, second, std::ignore) = four_two;
  std::cout << "what is the answer for " << first << " x " << second << " ?\n";

  std::cout << "\n";

  return 0;
}
