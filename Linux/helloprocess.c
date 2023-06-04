/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Copy standard input to output or files in argument.
 */

#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("Hello world from process ID %ld\n", (long)getpid());

  return 0;
}
