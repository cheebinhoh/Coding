/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program demonstrates some features of std::map
 * associative container.
 */

#include <iostream>
#include <map>

void printMap(std::map<int, std::string> &m) {
  for (auto &p : m) {
    std::cout << "key: " << p.first << ", value: " << p.second << "\n";
  }
}

int main(int argc, char *argv[]) {
  std::map<int, std::string> m{};

  std::cout << "Using [] to set english text for keys 0, 1 and 3, referencing "
               "2 without assigning value\n";
  m[0] = "zero";
  m[1] = "one";

  // note that we do not assign a value to key 2, so the value
  // has default initialization value which is empty string in this case.
  m[2];
  m[3] = "three";

  printMap(m);

  std::cout << "\n";
  std::cout << "Total # of elements in map is " << m.size() << "\n";

  std::cout << "\n";
  std::cout << "Using find to look up value for key 5\n";
  auto p = m.find(5);
  if (p == m.end()) {
    std::cout << "key 5 is not found\n";
  }

  std::cout << "\n";
  std::cout << "Total # of elements in map is " << m.size() << "\n";
  std::cout << "\n";

  std::cout << "Using find to look up value for key 1\n";
  p = m.find(1);
  if (p == m.end()) {
    std::cout << "key 1 is not found\n";
  } else {
    std::cout << "valeu for key 1 is " << p->second << "\n";
  }

  std::cout << "\n";
  std::cout << "Total # of elements in map is " << m.size() << "\n";
  std::cout << "\n";

  auto rp = m.equal_range(1);
  // map is not kept in contiguous storage (array or vector),
  // so we can not  use rp.second - rp.first to figure out the distance.
  if (std::distance(rp.first, rp.second) >= 1) {
    std::cout << "key 1 is found through map::range()\n";
  } else {
    std::cout << "key 1 is not found through map::range()\n";
  }

  std::cout << "\n";
  std::cout << "Inserting value for key 2 through std::map::insert\n";
  auto retp = m.insert(std::pair<int, std::string>(2, "two"));
  if (retp.second == false) {
    std::cout << "map already have key 2 with value " << retp.first->second
              << "\n";
  }

  std::cout << "\n";
  std::cout << "Inserting value for key 2 through []\n";
  m[2] = "two";
  std::cout << "key 2 has value " << m[2] << "\n";

  // std::map::emplace has same behavior as insert where it is only inserted
  // if key is not found.
  std::cout << "\n";
  std::cout << "Inserting value \"four\" for key 4 through emplace\n";
  m.emplace(4, "four");
  printMap(m);

  std::cout << "\n";

  return 0;
}
