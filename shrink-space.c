/* Copyright © 2021-2022 Chee Bin HOH. All rights reserved.
 *
 * It shrinks consecutive spaces into one space.
 */

#include <stdio.h>

int main(int argc, char *argv[]) {
  int c;
  int prevIsBlank = 0;

  while ((c = getchar()) != EOF) {
    if (' ' == c) {
      if (!prevIsBlank) {
        putchar(c);

        prevIsBlank = 1;
      }
    } else {
      putchar(c);

      prevIsBlank = 0;
    }
  }

  return 0;
}
