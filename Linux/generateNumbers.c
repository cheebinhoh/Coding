/* Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program just print the timestamp every 250 milliseconds for
 * 20 seconds.
 */

#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]) {
  int cnt;
  int seconds = 20;
  struct timespec now;
  struct timespec target;
  struct timespec prev;
  time_t now_time;
  struct tm *lctime;

  cnt = 0;
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
      cnt++;

      lctime = localtime(&now_time);
      strftime(strtime, sizeof(strtime), "%Y-%m-%d %I:%M:%S", lctime);
      printf("%d: %s %ld.%ld\n", cnt, strtime, now.tv_sec, now.tv_nsec);
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
