/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 */

#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <vector>

void printVector(const std::vector<int> &v) {
  int cnt;

  cnt = 0;
  for (const auto i : v) {
    if (cnt != 0) {
      std::cout << " ";
    }

    std::cout << i;
    cnt++;
  }

  std::cout << "\n";
}

void printList(const std::list<int> &l) {
  int cnt;

  cnt = 0;
  for (const auto i : l) {
    if (cnt != 0) {
      std::cout << " ";
    }

    std::cout << i;
    cnt++;
  }

  std::cout << "\n";
}

int main(int argc, char *argv[]) {
  std::cout << "*** std::vector\n";

  std::vector<int> v1 = {1, 2, 3, 4, 5};

  std::cout << "vector with 5 elements: ";
  printVector(v1);
  std::cout << "\n";

  v1.resize(7);
  std::cout << "resize vector to 7 elements with value initializer: ";
  printVector(v1);
  std::cout << "\n";

  v1.resize(4);
  std::cout << "resize vector to 4 elements with value initializer: ";
  printVector(v1);
  std::cout << "\n";

  v1.resize(6, 5);
  std::cout << "resize vector to 4 elements with value 5: ";
  printVector(v1);
  std::cout << "\n";

  std::vector<int>::iterator end = v1.end();
  end--;
  v1.erase(end);
  std::cout << "delete last element (6th) of vector: ";
  printVector(v1);
  std::cout << "\n";

  // the following approach is supposedly ill-function
  // because we erase iter prior to increment it, the
  // reason that it does not crash is because:
  // - std::vector is a sequential container that it has
  //   a dynamic array allocated internally
  // - when we delete any element, it moves the elements
  //   after that position downward to the array, but
  //   the iter still point to the existing N-th element
  //   though the content is different.
  //
  // and it does NOT work on std::list kind of container
  // that is pointer based, where erase an element involves
  // delete and free the element and rechain the connection
  // between elements via pointer.
  int cnt = 0;
  std::vector<int>::iterator iter = v1.begin();
  while (iter != v1.end()) {
    if (*iter % 2 == 0) {
      cnt++;
      v1.erase(iter);
    }

    iter++;
  }

  std::cout << "Number of even values is " << cnt << ", ";
  std::cout << "delete even value from vector: ";
  printVector(v1);
  std::cout << "\n";

  cnt = 0;
  std::remove_if(v1.begin(), v1.end(), [&cnt](int i) mutable {
    if (i != 3) {
      cnt++;
    }

    return i == 3;
  });

  // Quote from https://cplusplus.com/reference/algorithm/ that
  // the algorithm never affects the size or storage allocation of the
  // container). hence we need to delete the extra element left from deleting 3
  // out of 1, 3, 5 otherwise, it will have result 1, 5, 5 (left over as 5 in
  // 3rd position is moved up to 2nd position as we delete 3.

  v1.resize(cnt);
  std::cout << "Remove 3 from vector, it has " << cnt << " elements left: ";
  printVector(v1);
  std::cout << "\n";

  std::cout << "*** std::list\n";
  std::list<int> l1{1, 2, 3, 4, 5};
  std::cout << "list of values: ";
  printList(l1);

  cnt = 0;
  std::list<int>::iterator l_iter = l1.begin();
  while (l_iter != l1.end()) {
    std::list<int>::iterator tmp2del = l1.end();

    if (*l_iter % 2 == 0) {
      cnt++;
      tmp2del = l_iter;
    }

    l_iter++;

    if (tmp2del != l1.end()) {
      l1.erase(tmp2del);
    }
  }

  std::cout << "Number of even values is " << cnt << ", ";
  std::cout << "delete even value from list: ";
  printList(l1);
  std::cout << "\n";

  cnt = 0;
  std::remove_if(l1.begin(), l1.end(), [&cnt](int i) mutable {
    if (i != 3) {
      cnt++;
    }

    return i == 3;
  });

  l1.resize(cnt);
  std::cout << "Remove 3 from list, it has " << l1.size() << " elements left: ";
  printList(l1);
  std::cout << "\n";

  return 0;
}
