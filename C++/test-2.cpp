#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
  std::vector<int> l;
  std::vector<int>::iterator i;

  l.push_back(4);
  l.push_back(5);
  l.push_back(3);
  l.push_back(1);
  l.push_back(2);

  std::sort(l.begin(), l.end(),
            [=](const int &i1, const int &i2) {
    return i1 > i2;
  });

  i = l.begin();
  while (i != l.end()) {
    std::cout << *i << " ";
    i++;
  }

  std::cout << std::endl;

  return 0;
}
