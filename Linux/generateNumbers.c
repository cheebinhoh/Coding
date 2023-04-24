/* Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program just print the timestamp every 250 milliseconds for
 * 20 seconds.
 */

#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static char *programName = NULL;

void displayUsage(void) {
  printf("%s: [-s seconds]\n", programName);
  printf("Generate timestamp and random numbers for seconds\n");
  printf("\n");
  printf("options:\n");
  printf("  -s seconds  seconds must be 1 or more, default is 20\n");
}

int main(int argc, char *argv[]) {
  int opt;
  int seconds = 20;
  struct timespec now;
  struct timespec target;
  struct timespec prev;
  time_t now_time;
  struct tm *lctime;

  programName = basename(argv[0]);

  // parse command line arguments
  while ((opt = getopt(argc, argv, "s:h")) != -1) {
    switch (opt) {
    case 's':
      seconds = atoi(optarg);
      if (seconds <= 0) {
        displayUsage();
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

  prev.tv_sec = prev.tv_nsec = 0;
  clock_gettime(CLOCK_REALTIME, &now);
  target = now;
  target.tv_sec += seconds;

  while (now.tv_sec < target.tv_sec ||
         (now.tv_sec == target.tv_sec && now.tv_nsec <= target.tv_nsec)) {
    if (0 == prev.tv_sec ||
        (now.tv_sec == prev.tv_sec &&
         ((now.tv_nsec - prev.tv_nsec) >= (250 * 1000000))) ||
        (now.tv_sec > prev.tv_sec &&
         ((1000000000 - prev.tv_nsec) + now.tv_nsec) >= (250 * 1000000))) {
      char strtime[1000] = "\n";

      now_time = now.tv_sec;

      lctime = localtime(&now_time);
      strftime(strtime, sizeof(strtime), "%Y-%m-%d %I:%M:%S", lctime);
      printf("%s.%-10ld%c%ld\n", strtime, now.tv_nsec, '\t', random());
      fflush(stdout);

      prev = now;
    }

    clock_gettime(CLOCK_REALTIME, &now);
  }

  /** clock_gettime(CLOCK_REALTIME, &now);
    time_t now_time;
    struct timespec now;
    struct timespec target;
    struct timespec prev;
    struct tm *lctime;

    prev.tv_sec = 0;
    prev.tv_nsec = 0;

    clock_gettime(CLOCK_REALTIME, &now);
    target.tv_sec += elapsed_second;

    while (now.tv_sec <= target.tv_sec) {
      if (now.tv_sec > prev.tv_sec) {
        now_time = now.tv_sec;

        lctime = localtime(&now_time);
        printf("Current time is %s", asctime(lctime));
        prev = now;
      }

      clock_gettime(CLOCK_REALTIME, &now);
    }
   **/

  /** time_t
    time_t now;
    time_t target;
    time_t prev;
    struct tm *lctime;

    prev = 0;
    now = time(NULL);
    target = now + 2;

    while (now <= target) {
      if (now > prev) {
        lctime = localtime(&now);
        printf("Current time is %s", asctime(lctime));
      }

      now = time(NULL);
    }
   **/

  return 0;
}
