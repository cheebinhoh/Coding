/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Quick short, shell sort, insertion sort, bubble sort and binary search
 */

#include "search-sort.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  //             0  1  2  3  4
  int list[] = {5, 4, 3, 1, 2};
  int index;

  printf("-- is sorted = %d\n", isSorted(list, ARRAY_COUNT(list)));
  shellSort(list, ARRAY_COUNT(list));
  for (index = 0; index < ARRAY_COUNT(list); index++) {
    printf("%d ", list[index]);
  }

  printf("\n");

  printf("-- is sorted = %d\n", isSorted(list, ARRAY_COUNT(list)));

  return 0;
}
