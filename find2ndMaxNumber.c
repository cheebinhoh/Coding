/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * It trims trailing space or tab at the end of line or file.
 */

#include <stdio.h>

#define ARRAY_COUNT(array) (sizeof((array)) / sizeof((array)[0]))

int main(int argc, char *argvp[]) {
  int array[] = {3, 8, 1, 5, 9, 2, 6, 4, 7};
  int maxArray[2];
  int i;

  maxArray[0] = array[0];
  maxArray[1] = maxArray[0];

  for (i = 0; i < ARRAY_COUNT(array); i++) {
    if (array[i] > maxArray[0]) {
      maxArray[1] = maxArray[0];
      maxArray[0] = array[i];
    }
  }

  if (maxArray[0] > maxArray[1])
    printf("The 2nd maximum number is %d\n", maxArray[1]);

  return 0;
}
