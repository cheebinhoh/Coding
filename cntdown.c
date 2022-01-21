/* Copyright © 2022 Chee Bin HOH. All rights reserved.
 *
 * A program that returns number of seconds until target date time.
 * it is toy program created in respond to a friend leaving a bad company
 * and how I told him that I am going to create an app show the count down
 * until his last day.
 *
 * It exhibits Unix philosophy, the program does a single task, determine
 * number of seconds until the target datetime, it accepts date time
 * in the following strftime format %Y-%m-%d %H:%M:%S by default, but
 * user can specify a different format with -f argument, then it returns
 * the number of seconds by printing it, do one thing and do it well.
 *
 * I also added a companion front end shell program that can do fun thing
 * on top of this program, like calling this program repeatedly until count
 * down is zero, silent mode or other options.
 *
 * Happy coding!
 */

#define _XOPEN_SOURCE
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static char *baseProgName = NULL;

int main(int argc, char *argv[]) {
  time_t now;
  time_t targetTime;
  struct tm targetTm;
  char defaultFormat[] = "%Y-%m-%d %H:%M:%S";
  char *format = defaultFormat;
  char *stop;
  int opt;

  baseProgName = basename(argv[0]);

  while ((opt = getopt(argc, argv, "f:")) != -1) {
    switch (opt) {
    case 'f':
      format = optarg;
      break;

    default:
      fprintf(stderr, "Usage: %s [-f \"%s\"] \"2022-01-21 12:00:00\"\n", baseProgName,
              defaultFormat);
      exit(1);
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Usage: %s [-f \"%s\"] \"2022-01-21 12:00:00\"\n", baseProgName,
            defaultFormat);
    exit(1);
  }

  targetTm.tm_sec = 0;
  targetTm.tm_min = 0;
  targetTm.tm_hour = 0;
  targetTm.tm_mday = 0;
  targetTm.tm_mon = 0;
  targetTm.tm_yday = 0;
  targetTm.tm_year = 0;
  targetTm.tm_wday = 0;
  targetTm.tm_yday = 0;
  targetTm.tm_isdst = 0;

  stop = strptime(argv[optind], format, &targetTm);
  if (NULL == stop || '\0' != *stop) {
    fprintf(stderr, "invalid date time value\n");
    exit(2);
  }

  now = time(NULL);
  targetTime = mktime(&targetTm);
  printf("%ld\n", targetTime - now);

  return 0;
}
