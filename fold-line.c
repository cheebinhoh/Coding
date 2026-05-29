/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * It splits a long lines into multiple line according to max limit per line,
 * the split happens on space (\n, space or tab).
 */

#include <ctype.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLINE 80

char *programName = NULL;

void printHelp(void) {
  fprintf(stderr, "%s [-h] [-m line]\n", programName);
  fprintf(stderr, "\n");
  fprintf(stderr, " -h      : print this help message\n");
  fprintf(stderr,
          " -m line : number of character per line [default is 80 and maximum "
          "is %d]\n",
          BUFSIZ);
}

int main(int argc, char *argv[]) {
  int c;
  int maxline = MAXLINE;
  char buffer[BUFSIZ] = {'\0'};
  int count = 0;

  programName = basename(argv[0]);

  while ((c = getopt(argc, argv, "m:")) != -1) {
    switch (c) {
    case 'm':
      maxline = strtol(optarg, NULL, 0);
      if (maxline > sizeof(buffer)) {
        printHelp();
        exit(1);
      }

      break;

    case '?':
    case 'h':
      printHelp();
      exit(1);
    }
  }

  while (1) {
    if ((c = getchar()) == EOF) {
      int index = 0;
      while (index < count) {
        putchar(buffer[index++]);
      }

      break;
    } else if (count < maxline) {
      buffer[count++] = c;
    } else {
      int hasSpace = 1;
      int lastSpaceIndex = count - 1;
      if (!isspace(c)) {
        while (lastSpaceIndex >= 0 && !isspace(buffer[lastSpaceIndex])) {
          lastSpaceIndex--;
        }

        if (lastSpaceIndex < 0) {
          hasSpace = 0;
          lastSpaceIndex = count - 1;
        }
      }

      int index = 0;
      while (index <= lastSpaceIndex) {
        putchar(buffer[index++]);
      }

      index = 0;
      lastSpaceIndex++;
      while (lastSpaceIndex < count) {
        buffer[index++] = buffer[lastSpaceIndex++];
      }

      if (isspace(c)) {
        count = 0;
      } else {
        buffer[index++] = c;
        count = index;
      }

      // if we do not found space to split, we do not want to newline, as next
      // character will join with data in buffer we just print
      if (hasSpace) {
        putchar('\n');
      }
    }
  } /* while ((c = getchar()) != EOF) */

  return 0;
}
