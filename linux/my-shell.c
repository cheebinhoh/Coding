/**
 * Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Bare shell program to execute other program
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  char buf[BUFSIZ];
  int n;
  pid_t pid;
  int status;

  printf("%% ");
  while (fgets(buf, sizeof(buf), stdin) != NULL) {
    if ('\n' == buf[strlen(buf) - 1]) {
      buf[strlen(buf) - 1] = '\0';
    }

    if ((pid = fork()) < 0) {
      fprintf(stderr, "error in fork: %s\n", strerror(errno));
      exit(1);
    } else if (pid == 0) {
      execlp(buf, buf, (char *)NULL);
      fprintf(stderr, "error in exec: %s\n", strerror(errno));
      exit(127);
    }

    if ((pid = waitpid(pid, &status, 0)) < 0) {
      fprintf(stderr, "error in waitpid: %s\n", strerror(errno));
      exit(1);
    }

    printf("%% ");
  }

  exit(0);
}
