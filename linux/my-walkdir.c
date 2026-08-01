/**
 * Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Bare directory walking, just print file under directory for now
 */

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <libgen.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef void DirWalkCallback(char *filepath, struct stat *stat);

static char *progName = NULL;

static DirWalkCallback dirWalkCb;
static void printUsage(FILE *);
static bool walkDir(char *dirpath, DirWalkCallback cb);

int main(int argc, char *argv[]) {
  struct option long_options[] = {{"help", no_argument, 0, 'h'}, {0, 0, 0, 0}};

  progName = basename(argv[0]);

  int c;
  while ((c = getopt_long(argc, argv, "h", long_options, NULL)) != -1) {
    switch (c) {
    case 'h':
      printUsage(stdout);
      exit(0);

    case '?':
      printUsage(stderr);
      exit(1);
    }
  }

  if (optind >= argc) {
    printUsage(stderr);
    exit(1);
  }

  bool ret = walkDir(argv[optind], dirWalkCb);

  return ret ? 0 : 1;
}

void dirWalkCb(char *filepath, struct stat *stat) {
  if (S_ISDIR(stat->st_mode)) {
    walkDir(filepath, dirWalkCb);
  } else {
    printf("%s\n", filepath);
  }
}

static void printUsage(FILE *out) {
  assert(NULL != out);

  fprintf(out, "%s: usage: %s [directory|file]\n", progName, progName);
}

bool walkDir(char *dirpath, DirWalkCallback cb) {
  DIR *dir = NULL;
  struct dirent *dirent = NULL;
  bool ret = false;
  char filepath[PATH_MAX] = "";
  char dirpathLen = 0;

  assert(NULL != dirpath);

  dir = opendir(dirpath);
  if (NULL == dir) {
    fprintf(stderr, "%s: Error in opendir %s: %s\n", progName, dirpath,
            strerror(errno));
    goto cleanup_on_return;
  }

  dirpathLen = strlen(dirpath);
  strncpy(filepath, dirpath, sizeof(filepath));
  assert(filepath[sizeof(filepath) - 1] == '\0');

  if (dirpath[dirpathLen - 1] == '/') {
    dirpathLen--;
    filepath[dirpathLen] = '\0';
  }

  while ((dirent = readdir(dir)) != NULL) {
    struct stat fileStat;

    if (strcmp(".", dirent->d_name) == 0 || strcmp("..", dirent->d_name) == 0) {
      continue;
    }

    strncpy(filepath + dirpathLen, "/", sizeof(filepath) - dirpathLen);
    assert(filepath[sizeof(filepath) - 1] == '\0');

    strncpy(filepath + dirpathLen + 1, dirent->d_name,
            sizeof(filepath) - (dirpathLen + 1));
    assert(filepath[sizeof(filepath) - 1] == '\0');

    if (stat(filepath, &fileStat) == -1) {
      fprintf(stderr, "%s: Error stat %s: %s\n", progName, filepath,
              strerror(errno));
      goto cleanup_on_return;
    }

    cb(filepath, &fileStat);

    filepath[dirpathLen] = '\0';
  }

  ret = true;

cleanup_on_return:
  closedir(dir);

  return ret;
}
