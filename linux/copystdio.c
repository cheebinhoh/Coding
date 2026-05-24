/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Copy standard input to output or files in argument.
 */

#include <stdio.h>

int main(int argc, char *argv[]) {
  int c;

  while ((c = getc(stdin)) != EOF) {
    putc(c, stdout);
  }

  return 0;
}
