/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * Quick short, shell sort, insertion sort, bubble sort and binary search.
 */

#include "search-sort.h"
#include <stdio.h>

int isSorted(int list[], int size) {
  int result = 1;
  int i;

  for (i = 1; i < size - 1; i++) {
    if (list[i - 1] > list[i]) {
      result = 0;

      break;
    }
  }

  return result;
}

/* Quick sort
 *
 * The algorithm is that:
 * - the right most element is picked as a pivot.
 * - then we compare pivot element to the left indexed element (start from the
 * left at zero index).
 * --- if the left element is smaller than the pivot element (from right), we
 *     increase the index of left element by 1.
 * --- if the left element is larger than the pivot element, we move the left
 *     element to the pivot element to the left indexed element position, we
 * move the pivot element into the left position of the pivot element, and we
 *     relocate original left indexed element to the right of the new pivot
 * position
 * --- we repeat above as long as left index is smaller than pivot position
 * - once we stop that, we know that everything to the left of the pivot will be
 *   smaller, and everything to the right will be larger than pivot element.
 * - then we recursively invoke quick sort on left and right elements of the
 *   pivot.
 *
 * Big(O) notation:
 * - for best and average case, it is O(n log n) when the selected pivot element
 *   can divide the array averagely into half.
 * - for worst case that the pivot element is the largest value or smallest
 * value, we will have unevenly divided subarray for further sorting.
 */
void quickSort(int list[], int size) {
  int pivot = size - 1;
  int i = 0;
  int tmp;

  while (i < pivot) {
    if (list[i] <= list[pivot]) {
      i++;
    } else {
      // pivot is moved to position pivot - 1
      // original item at position pivot - 1 is moved to the position ith
      // original item at position ith is moved to the original pivot position
      //
      // at the end of this iteration, the ith element will be sorted in related
      // to pivot element

      tmp = list[i];
      list[i] = list[pivot - 1];
      list[pivot - 1] = list[pivot];
      list[pivot] = tmp;

      pivot--;
    }
  }

  if (pivot > 1) {
    quickSort(list, pivot);
  }

  if (size - pivot - 1 > 1) {
    quickSort(&list[pivot + 1], size - pivot - 1);
  }
}

/* Shell sort
 *
 * Shell sort is like bubble sort and compare a pair of elements and sort them
 * into proper ordering, however instead of compare adjacent elements, shell
 * sort will start off with a pair of elements separated apart by bigger gap,
 * and then repeatedly decrease the gap and then eventually sort adjacent pair,
 * bubble reduces the # of swap that happened.
 */
void shellSort(int list[], int size) {
  int gap;
  int j;
  int i;
  int tmp;
  int count = 0;

  for (gap = size / 2; gap > 0; gap = gap / 2) {
    for (j = gap; j < size; j++) {
      for (i = j - gap; i >= 0 && list[i] > list[i + gap]; i = i - gap) {
        tmp = list[i];
        list[i] = list[i + gap];
        list[i + gap] = tmp;

        count++;
      }
    }
  }
}

/* Insertion sort
 *
 * loop through each item via i
 * compare all elements before i and insert i in proper position
 */
void insertionSort(int list[], int size) {
  int i;
  int j;
  int tmp;
  int count = 0;

  for (i = 1; i < size; i++) {
    tmp = list[i];

    for (j = i - 1; j >= 0 && list[j] > tmp; j--) {
      list[j + 1] = list[j];

      count++;
    }

    list[j + 1] = tmp;
  }
}

/* Bubbles sort
 */
void bubbleSort(int list[], int size) {
  int i;
  int j;
  int tmp;
  int count;

  for (i = 0; i < size - 1; i++) {
    for (j = 0; j < size - 1 - i; j++) {
      if (list[j] > list[j + 1]) {
        tmp = list[j];
        list[j] = list[j + 1];
        list[j + 1] = tmp;

        count++;
      }
    }
  }
}

/* It is a simple O(n^2) sorting algorithm
 */
void selectionSort(int list[], int size) {
  int i;

  for (i = 0; i < size; i++) {
    int smallestIndex = i;
    int j;

    for (j = i + 1; j < size; j++) {
      if (list[smallestIndex] > list[j]) {
        smallestIndex = j;
      }
    }

    if (smallestIndex != i) {
      int tmp = list[smallestIndex];
      list[smallestIndex] = list[i];
      list[i] = tmp;
    }
  }
}

int binarySearchOnRotateList(int list[], int size, int rotatedStep, int value) {
  int head = 0;
  int tail = size - 1;
  int middle = (head + tail) / 2;
  int index;

  while (head <= tail) {
    index = (rotatedStep + middle) % size;

    if (list[index] == value)
      return index;
    else if (value < list[index])
      tail = middle - 1;
    else
      head = middle + 1;

    middle = (head + tail) / 2;
  }

  return -1;
}

/* Binary search
 */
int binarySearch(int list[], int size, int value) {
  int rotatedStep = 0;
  int prev;
  int i;

  // find out how much has been rotated.
  if (size > 1) {
    prev = list[0];

    for (i = 1; i < size; i++) {
      if (prev > list[i]) {
        rotatedStep = i;
        break;
      }

      prev = list[i];
    }
  }

  return binarySearchOnRotateList(list, size, rotatedStep, value);
}

void printIntegerArray(int array[], int size) {
  int i;

  for (i = 0; i < size - 1; i++)
    printf("%d, ", array[i]);

  if (size >= 1)
    printf("%d\n", array[size - 1]);
}
