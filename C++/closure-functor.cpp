/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program demonstrates the use of closure to create a functor object
 * IDGenerator that will keep a running counter and return next id available
 * and the functor allows us to pass in customized closure that generate next
 * id after return one.
 */

#include <iostream>

class IDGenerator {
public:
  IDGenerator(long start) : next{start} {}

  IDGenerator() : IDGenerator{0} {}

  long operator()(long (*nextIDFunc)(long)) {
    long id = next;

    next = (*nextIDFunc)(next);

    return id;
  }

  long operator()() {
    return (*this)([](long curr) { return curr + 1; });
  }

private:
  long next{0};
};

int main(int argc, char *argv[]) {
  IDGenerator id1{1};
  IDGenerator id2{2};

  std::cout << "Next id for id1 is " << id1() << "\n";
  std::cout << "Next id for id1 is " << id1() << "\n";
  std::cout << "\n";

  std::cout << "Next id for id2 is " << id2([](long curr) { return curr + 2; })
            << "\n";
  std::cout << "Next id for id2 is " << id2([](long curr) { return curr + 2; })
            << "\n";

  return 0;
}
