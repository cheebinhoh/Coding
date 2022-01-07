/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Copy file from input to output, like cp and cat.
 */

#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  char c;

  while (!cin.eof()) {
    cin.get(c);
    cout << c;
  }

  return 0;
}
