/* Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program just read standard input and write to standard output.
 */

#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static char *programName = NULL;

void displayUsage(void) {
  printf("%s: [-f file]\n", programName);
  printf("Read timestamp and random numbers\n");
  printf("\n");
  printf("options:\n");
  printf("  -f file  file to read the content from, default is stdin\n");
}

int main(int argc, char *argv[]) {
  int opt;
  char s[1024];
  FILE *fptr = stdin;

  programName = basename(argv[0]);

  // parse command line arguments
  while ((opt = getopt(argc, argv, "f:h")) != -1) {
    switch (opt) {
    case 'f':
      fptr = fopen(optarg, "r");
      if (NULL == fptr) {
        perror(programName);
        exit(1);
      }
      break;

    case '?':
      displayUsage();
      exit(1);
      break;

    default:
      assert("unhandled option");
      break;
    }
  }

  while (NULL != fgets(s, sizeof(s), fptr)) {
    fputs(s, stdout);
  }

  return 0;
}
