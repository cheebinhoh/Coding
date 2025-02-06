#include <iostream>

void f(int &&i) { std::cout << "rvalue\n"; }

void f(int &i) { std::cout << "lvalue\n"; }

class LoR {
public:
  void print() & { std::cout << "lvalue\n"; }

  void print() && { std::cout << "rvalue\n"; }
};

int main(int argc, char *argv[]) {
  int i{5};

  f(i);
  f(6);

  LoR lor{};

  lor.print();

  (LoR{}).print();

  return 0;
}
