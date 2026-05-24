/**
 * Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Copy standard input to output or files in argument.
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int c;

  while ((c = getc(stdin)) != EOF) {
    if (putc(c, stdout) == EOF) {
      exit(1);
    }
  }

  if (ferror(stdin)) {
    exit(1);
  }

  exit(0);
}
