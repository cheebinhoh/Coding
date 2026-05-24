/**
 * Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Copy standard input to output or files in argument.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int c;

  while ((c = getc(stdin)) != EOF) {
    if (putc(c, stdout) == EOF) {
      fprintf(stderr, "putc error: %s\n", strerror(errno));
      exit(1);
    }
  }

  if (ferror(stdin)) {
    fprintf(stderr, "getc error: %s\n", strerror(errno));
    exit(1);
  }

  exit(0);
}
