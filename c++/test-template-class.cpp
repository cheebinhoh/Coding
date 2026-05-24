#include "template-class.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  const SampleTemplate<int> sample{5};

  std::cout << "value: " << sample.GetMember() << "\n";

  return 0;
}
