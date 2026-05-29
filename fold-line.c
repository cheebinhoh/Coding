/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * It splits a long lines into multiple line according by max limit of character
 * per line the split happens on space (\n, space or tab), note that \n in input
 * is observed.
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

  int skipLeadingSpace = 0;
  while (1) {
    if ((c = getchar()) == EOF) {
      // if it is end of input, print everything not yet print
      for (int index = 0; index < count; index++) {
        putchar(buffer[index]);
      }

      break;
    } else if (count < maxline) {
      // accummulate characters for split when reach max number of characters
      // but skip leading space if indicated by split in prior line.
      if (skipLeadingSpace && isspace(c)) {
        ;
      } else {
        buffer[count++] = c;
        skipLeadingSpace = 0;
      }
    } else {
      // print everything up to last space
      // shift characters after last space
      // insert new character
      int lastSpaceIndex = count - 1;
      if (!isspace(c)) {
        while (lastSpaceIndex >= 0 && !isspace(buffer[lastSpaceIndex])) {
          lastSpaceIndex--;
        }

        if (lastSpaceIndex < 0) {
          lastSpaceIndex = count - 1;
        }
      }

      int index = 0;
      while (index <= lastSpaceIndex) {
        putchar(buffer[index++]);
      }

      if (isspace(c) ||
          (lastSpaceIndex < count && isspace(buffer[index - 1]))) {
        putchar('\n');
        skipLeadingSpace = isspace(c);
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
    }
  } /* while ((c = getchar()) != EOF) */

  return 0;
}
