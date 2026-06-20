/**
 * Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Bare shell program to execute other program
 */

#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  bool islseekable = lseek(STDIN_FILENO, 0, SEEK_CUR) == 0;

  if (!islseekable) {
    fprintf(stderr, "Error: %s\n", strerror(errno));
  }

  return islseekable ? 0 : 1;
}
