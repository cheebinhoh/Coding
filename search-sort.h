/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * Quick short, shell sort, insertion sort, bubble sort and binary search.
 */

#ifndef SEARCH_SORT_H_HAVE_SEEN

#define SEARCH_SORT_H_HAVE_SEEN

#define ARRAY_COUNT(array) (sizeof((array)) / sizeof((array)[0]))

void bubbleSort(int list[], int size);
void insertionSort(int list[], int size);
void shellSort(int list[], int size);
void quickSort(int list[], int size);
int binarySearch(int list[], int size, int value);
int isSorted(int list[], int size);

void printIntegerArray(int array[], int size);

#endif
