/* Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program just read standard input and write to standard output.
 */

#include <stdio.h>

int main(int argc, char *argv[]) {
  char s[1024];

  while (NULL != fgets(s, sizeof(s), stdin)) {
    fputs(s, stdout);
  }

  return 0;
}
