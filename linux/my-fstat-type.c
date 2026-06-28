/**
 * Copyright © 2026 Chee Bin HOH. All rights reserved.
 *
 * Print fstat types of arguments to the program
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
  struct stat st;
  
  for (int i = 1; i < argc; i++) {
    printf("%s: ", argv[i]);

    if (lstat(argv[i], &st) < 0) {
      printf("Error: %s\n", strerror(errno));
      continue;
    }

    if (S_ISREG(st.st_mode)) {
      printf("regular file\n");
    } else if (S_ISDIR(st.st_mode)) {
      printf("directory\n");
    } else if (S_ISCHR(st.st_mode)) {
      printf("character special\n");
    } else if (S_ISBLK(st.st_mode)) {
      printf("block special\n");
    } else if (S_ISFIFO(st.st_mode)) {
      printf("fifo\n");
    } else if (S_ISLNK(st.st_mode)) {
      printf("symbolic link\n");
    } else if (S_ISSOCK(st.st_mode)) {
      printf("socket\n");
    } else {
      printf("Error: unknown\n");
    }
  }
  
  return 0;
}
