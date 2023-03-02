#include <stdio.h>

int isSorted(int array[], int size) {
  int prev;
  int i;

  prev = array[0];
  i = 1;
  while (i < 5) {
    if (array[i] < prev) {
      return 0;
    }

    prev = array[i];
    i++;
  }

  return 1;
}

int main(void) {
  int array1[] = {1, 2, 3, 4, 5};
  int array2[] = {2, 3, 4, 1, 5};

  printf("is array1 sorted? %d\n", isSorted(array1, 5));
  printf("is array2 sorted? %d\n", isSorted(array2, 5));

  return 0;
}
