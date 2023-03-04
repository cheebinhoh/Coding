/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 */

#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *programName = NULL;

int main(int argc, char *argv[]) {
  DIR *dp;
  struct dirent *dirp;

  programName = basename(argv[0]);

  if (argc != 2) {
    fprintf(stderr, "Usage: %s directory\n", programName);
    exit(1);
  }

  if ((dp = opendir(argv[1])) == NULL) {
    fprintf(stderr, "%s: %s\n", programName, strerror(errno));
    exit(2);
  }

  while ((dirp = readdir(dp)) != NULL)
    printf("%s\n", dirp->d_name);

  closedir(dp);

  return 0;
}
