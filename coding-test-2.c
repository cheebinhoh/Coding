/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * Coding test questions from Amazon (at least internet claim it :)) and many
 * others.
 *
 * https://www.educative.io/blog/crack-amazon-coding-interview-questions#questions
 * questions that is similar I come across from friend or online.
 *
 * All answers are done by me with some assumption sometimes to make it easy for
 * me :) I do some reading about some topic to refresh my knowledge about
 * certain theory, like AVL before I start coding it.
 *
 * Happy coding!
 */

#include "btree.h"
#include "llist.h"
#include "search-sort.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <time.h>

/* Test 1:
 *
 * you are given an array of positive numbers from 1 to n, such that all numbers
 * from 1 to n are present except one number x. You have to find x. The input
 * array is not sorted, O(n).
 */
int findMissingNumberInTheArray(int array[], int size, int n) {
  int sum;
  int expectedSum;
  int i;

  expectedSum = 0;
  for (i = 1; i <= n; i++)
    expectedSum += i;

  sum = 0;
  for (i = 0; i < size; i++)
    sum += array[i];

  return (expectedSum - sum);
}

/* Assume that the array is sorted and numbers are 1 or above. This
 * function will return 0 if no missing number.
 *
 * A simple recursive approach that divides the search space and
 * locate the missing number, O(log n).
 */
int findMissingNumberInTheSortedArray(int array[], int size) {
  int mid;
  int res;

  assert(size > 0);

  if (size <= 1 || ((array[0] + size - 1) == array[size - 1])) {
    return -1;
  }

  mid = (size / 2) - 1;
  assert(mid >= 0 && mid < size - 1);

  if (array[mid] + 1 != array[mid + 1]) {
    return array[mid] + 1;
  } else if (mid > 0 && array[mid - 1] + 1 != array[mid]) {
    return array[mid - 1] + 1;
  }

  res = findMissingNumberInTheSortedArray(&array[0], mid + 1);
  if (-1 == res) {
    res = findMissingNumberInTheSortedArray(&array[mid + 1], size - (mid + 1));
  }

  return res;
}

void runFindMissingNumberInTheArray(void) {
  int array1[] = {3, 7, 1, 2, 8, 4, 5};
  int array2[] = {1, 2, 3, 4, 5, 7, 8};

  printf("Test 1: find the missing number in the array.\n");
  printf("\n");

  printf("The integer array is ");
  printIntegerArray(array1, sizeof(array1) / sizeof(array1[0]));
  printf("The missing integer is %d\n",
         findMissingNumberInTheArray(array1, sizeof(array1) / sizeof(array1[0]),
                                     8));
  printf("\n");

  printf("The integer array is sorted: ");
  printIntegerArray(array2, sizeof(array2) / sizeof(array2[0]));
  printf("The missing integer is %d\n",
         findMissingNumberInTheSortedArray(array2,
                                           sizeof(array2) / sizeof(array2[0])));
}

/*
 * Test 2:
 *
 * Given an array of integers and a value, determine if there are any two
 * integers in the array whose sum is equal to the given value. Return true if
 * the sum exists and return false if it does not.
 */
void print2NumberSumToValue(int array[], int size, int val) {
  int i;
  int remaining;
  struct TreeNode *root;
  struct TreeNode *node;
  struct TreeNode *foundList;

  root = NULL;
  for (i = 0; i < size; i++)
    root = addTreeNode(root, array[i]);

  node = NULL;
  foundList = NULL;
  for (i = 0; i < size; i++) {
    // if we have 1, 2, 3, ... 9, 10, and we want to find sum of two to 7
    // we will find 3 and 7, this check will avoid repeating it by finding 7
    // which is already found when 3 is a pivot value in early loop.

    node = findTreeNode(foundList, array[i]);
    if (NULL != node)
      continue;

    remaining = val - array[i];

    node = findTreeNode(root, remaining);
    if (NULL != node && node->val != array[i]) {
      foundList = addTreeNode(foundList, array[i]);
      foundList = addTreeNode(foundList, node->val);

      printf("[%d, %d] ", array[i], node->val);
    }
  }

  freeTreeNode(root);
  freeTreeNode(foundList);

  printf("\n");
}

void runDeteremineIf2NumberSumToValue(void) {
  int array1[] = {5, 7, 1, 2, 8, 4, 3};
  int found;

  printf("Test 2: determine if the sum of two integers is equal to the given "
         "value.\n");
  printf("\n");

  printf("The integer array is ");
  printIntegerArray(array1, sizeof(array1) / sizeof(array1[0]));
  printf("The list of two numbers added to 10 is ");
  print2NumberSumToValue(array1, sizeof(array1) / sizeof(array1[0]), 10);
  printf("\n");
}

/* Test 3:
 *
 * Given a dictionary of words and a large input string. You have to find out
 * whether the input string can be completely segmented into the words of a
 * given dictionary.
 */
int determineIfStringCanBeSegmented(char *dictionary[], char *s) {
  int i;
  int len;
  int can;

  for (i = 0; NULL != dictionary[i]; i++) {
    len = strlen(dictionary[i]);

    if (strncmp(dictionary[i], s, len) == 0) {
      if ('\0' == s[len])
        return 1;
      else if (determineIfStringCanBeSegmented(dictionary, s + len))
        return 1;

      // recursive method can be expensive and depending on the string to be
      // segmented, we can be stack overflow, another way is to maintain
      // backtracking explicitly by storing the previous position in a list or
      // array, so that if next search and match does not work, we backtrack to
      // previous position in string to explore into other search.
    }
  }

  return 0;
}

int determineIfStringCanBeSegmentedIterative(char *dictionary[], char *s) {
  int i;
  int j;
  int len;
  struct ListNode *sPosStack;
  struct ListNode *dPosStack;
  struct ListNode *tmp;

  sPosStack = NULL;
  dPosStack = NULL;
  j = 0;
  i = 0;
  while (NULL != dictionary[i]) {
    len = strlen(dictionary[i]);

    if (strncmp(dictionary[i], s + j, len) == 0) {
      if ('\0' == *(s + j + len)) {
        goto success;
      } else {
        pushStackInt(j, &sPosStack);
        j += len;

        pushStackInt(i + 1, &dPosStack);
        i = 0;

        continue;
      }

      // recursive method can be expensive and depending on the string to be
      // segmented, we can be stack overflow, another way is to maintain
      // backtracking explicitly by storing the previous position in a list or
      // array, so that if next search and match does not work, we backtrack to
      // previous position in string to explore into other search.
    }

    i++;

    if (NULL == dictionary[i]) {
      if (getListLength(dPosStack) > 0) {
        tmp = popStack(&sPosStack);
        j = tmp->data.val;
        free(tmp);

        tmp = popStack(&dPosStack);
        i = tmp->data.val;
        free(tmp);
      }
    }
  }

  return 0;

success:
  freeList(&sPosStack);
  freeList(&dPosStack);

  return 1;
}

void runStringSegmentation(void) {
  char *dictionary[] = {"apple", "pear", "pie", NULL};
  char s1[] = "applepie";
  char s2[] = "applepeer";
  int i;

  printf("Test 3: given a dictionary of words and a large input string. You "
         "have to find out "
         "whether the input string can be completely segmented into the words "
         "of a given dictionary.\n");
  printf("\n");

  printf("The dictionary words are: ");
  for (i = 0; NULL != dictionary[i]; i++)
    printf("\"%s\" ", dictionary[i]);

  printf("\n");

  if (determineIfStringCanBeSegmentedIterative(dictionary, s1))
    printf("The string \"%s\" can be segmented\n", s1);
  else
    printf("The string \"%s\" can not be segmented\n", s1);

  if (determineIfStringCanBeSegmentedIterative(dictionary, s2))
    printf("The string \"%s\" can be segmented\n", s2);
  else
    printf("The string \"%s\" can not be segmented\n", s2);

  printf("\n");
}

/*
 * Test 4:
 *
 * Given a sorted array of integers, return the low and high index of the given
 * key. You must return -1 if the indexes are not found. The array length can be
 * in the millions with many duplicates.
 */

int findLowHighIndex(int array[], int size, int val, int *low, int *high) {
  int s;
  int e;
  int mid;
  int foundIndex;
  int startIndex;

  *low = -1;
  *high = -1;
  foundIndex = binarySearch(array, size, val);
  if (-1 == foundIndex)
    return foundIndex;

  // find the low
  s = 0;
  e = foundIndex - 1;
  while (s < e) {
    mid = (e - s) / 2;
    if (val <= array[mid])
      e = mid - 1;
    else
      s = mid + 1;
  }

  *low = e + 1;

  // find the high
  s = foundIndex + 1;
  e = size - 1;
  startIndex = s;
  while (e > s) {
    mid = s + ((e - s) / 2);
    if (val >= array[mid])
      s = mid + 1;
    else
      e = mid - 1;
  }

  *high = s - 1;

  return *low;
}

void runFindLowHighIndex(void) {
  int array[] = {1, 2, 5, 5, 5, 5, 5, 5, 5, 5, 20};
  int low;
  int high;

  printf("Test 4: given a sorted array of integers, return the low and high "
         "index of the given key. "
         "You must return -1 if the indexes are not found. The array length "
         "can be in the "
         "millions with many duplicates.\n");
  printf("\n");

  printf("The integer array is ");
  printIntegerArray(array, sizeof(array) / sizeof(array[0]));
  findLowHighIndex(array, sizeof(array) / sizeof(array[0]), 5, &low, &high);
  if (-1 != low)
    printf("For value %d, the low index is %d and high index is %d\n", 5, low,
           high);

  printf("\n");
}

/*
 * Test 5:
 *
 * find k largest(or smallest) elements in an array
 */
void printKLargestElementsInArray(int array[], int size, int k) {
  int i;
  int n;
  struct ListNode *start = NULL;

  n = 0;
  for (i = 0; i < size; i++) {
    if (n < k) {
      addListNodeInt(array[i], &start, NULL);
      n++;
    } else if (array[i] > start->data.val) {
      delListNodeInt(start->data.val, &start, NULL);
      addListNodeInt(array[i], &start, NULL);
    }
  }

  printf("The largest %d values are :", k);
  printListNodeInt(start);
  freeList(&start);
}

void runFindKLargestElementsInArray(void) {
  int array1[] = {1, 23, 12, 9, 30, 2, 50};

  printf("Test 5: find k largest(or smallest) elements in an array.\n");
  printf("\n");

  printf("The integer array is ");
  printIntegerArray(array1, sizeof(array1) / sizeof(array1[0]));
  printKLargestElementsInArray(array1, sizeof(array1) / sizeof(array1[0]), 3);
  printf("\n");
}

/*
 * Test 6:
 *
 * Rotate array by k elements
 */
void rotateArrayByKElement(int array[], int size, int k) {
  int i;
  struct ListNode *start = NULL;
  struct ListNode *tmp;

  for (i = 0; i < k && i < size; i++)
    addListNodeInt(array[i], &start, NULL);

  for (i = 0; i < size - k; i++) {
    array[i] = array[i + k];
  }

  tmp = start;
  while (NULL != tmp) {
    array[i++] = tmp->data.val;
    tmp = tmp->next;
  }

  freeList(&start);
}

void runRotateArrayByKElement(void) {
  int array1[] = {1, 2, 3, 4, 5, 6, 7};

  printf("Test 6: rotate array by k elements\n");
  printf("\n");
  printf("The integer array is ");
  printIntegerArray(array1, sizeof(array1) / sizeof(array1[0]));
  rotateArrayByKElement(array1, sizeof(array1) / sizeof(array1[0]), 2);

  printf("\n");
  printf("After rotating by 2, the integer array is ");
  printIntegerArray(array1, sizeof(array1) / sizeof(array1[0]));
  printf("\n");
}

/*
 * Test 7:
 *
 * Rotate matrix 90" degree to right.
 */
void printArrayInMatrix(int array[], int size, int matrixSize) {
  int r;
  int c;
  int rIndex;

  for (r = 0; r < matrixSize; r++) {
    rIndex = r * matrixSize;

    for (c = 0; c < matrixSize; c++) {
      printf("%2d     ", array[rIndex + c]);
    }

    printf("\n");
  }
}

/* This method is memory effective as it only requires matrixSize of extra
 * buffer, so if we have a 16 x 16 matrix, it only requires 16 int buffer, than
 * 16 x 16.
 *
 * The logic is that:
 * - we will form the new first row by picking up number backward from the list
 * that need to be in the new first row.
 * - as a number is picked and put into buffer to be part of new first row, we
 * will move the prior number down the list, so we free up space in first row of
 * the list
 * - as 3 numbers are  picked for 1st row, the first row of the list will have
 * freed up for the new first row of numbers.
 *
 * Note that the new first row is NOT the old last row because we rotate to
 * right by 90" degree, instead first loop, the new first row is 7, 4, 1 which
 * is separated by 3 spaces (exactly the matrix size), after picking up one
 * number, we will move number prior to it down the list.
 *
 * After picking first row, the 2nd row number started backward from the list at
 * backward position matrix - 1 (2nd row index) and separate by 2 space (matrix
 * size 3 - 1, 2nd row).
 *
 * Example, new first row is marked by + sign.
 *
 * 3 x 3 matrix:
 * 1+     2      3
 * 4+     5      6
 * 7+     8      9
 *
 * after first row is moved, the matrix is like this:
 *
 * 7      4      1
 * 2+     3      5+
 * 6      8+     9
 *
 * new 2nd row is 8, 5 and 2, backward by 2 space, so new matrix is then:
 *
 * 7      4      1
 * 8      5      2
 * 3+     6+     9+
 *
 * new 3rd row is 9, 6 and 3, backward by 1 space, so we pick 9, 6 and 3 into
 * 3rd row, so the final matrix is:
 *
 * 7      4      1
 * 8      5      2
 * 9      6      3
 */
void rotateMatrix90DegreeToRight(int array[], int size, int matrixSize) {
  int i;
  int j;
  int k;
  int l;
  int *buffer;

  buffer = malloc(sizeof(int) * matrixSize);
  if (NULL == buffer)
    return;

  l = 0;
  for (i = 0; i < matrixSize; i++) {
    k = size - (matrixSize - i);
    for (j = 0; j < matrixSize; j++) {
      buffer[j] = array[k];

      if (k - j - (i * matrixSize) > 0)
        memmove(&array[l + 1], &array[l],
                (k - j - (i * matrixSize)) * sizeof(int));

      k = k - (matrixSize - i - 1);
      l++;
    }

    for (j = 0; j < matrixSize; j++) {
      array[l - matrixSize + j] = buffer[j];
    }
  }

  free(buffer);
}

void runRotateMatrix90DegreeToRight(void) {
  int array1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  int array2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

  printf("Test 7: rotate matrix 90\" degree to right.\n");
  printf("\n");

  printf("3 x 3 matrix:\n");
  printArrayInMatrix(array1, sizeof(array1) / sizeof(array1[0]), 3);
  printf("\n");

  rotateMatrix90DegreeToRight(array1, sizeof(array1) / sizeof(array1[0]), 3);

  printf("After rotating matrix 90\" degree to right:\n");
  printArrayInMatrix(array1, sizeof(array1) / sizeof(array1[0]), 3);
  printf("\n");

  printf("4 x 4 matrix:\n");
  printArrayInMatrix(array2, sizeof(array2) / sizeof(array2[0]), 4);
  printf("\n");

  rotateMatrix90DegreeToRight(array2, sizeof(array2) / sizeof(array2[0]), 4);

  printf("After rotating matrix 90\" degree to right:\n");
  printArrayInMatrix(array2, sizeof(array2) / sizeof(array2[0]), 4);
  printf("\n");
}

/*
 * Test 8:
 *
 * Merge two lists.
 */
void runMergeTwoLists(void) {
  struct ListNode *first = NULL;
  struct ListNode *second = NULL;
  struct ListNode *merge = NULL;

  printf("Test 8: merge two lists\n");
  printf("\n");

  enQueueInt(1, &first);
  enQueueInt(2, &first);
  enQueueInt(5, &first);
  enQueueInt(6, &first);

  enQueueInt(2, &second);
  enQueueInt(3, &second);
  enQueueInt(4, &second);
  enQueueInt(7, &second);

  printf("first list is:  ");
  printListInt(first);

  printf("second list is: ");
  printListInt(second);
  printf("\n");

  merge = mergeSortedListInt(first, second);
  printf("merge list is: ");
  printListInt(merge);
  printf("\n");
}

/*
 * Test 9:
 *
 * Remove vowels from String
 */
void removeVowelsFromString1(char s[]) {
  int len;
  int i;
  int j;
  int c;
  int gap;

  len = strlen(s);
  i = len - 1;
  j = i;

  while (i >= 0) {
    c = tolower(s[i]); // i == j at this point
    while (j >= 0 &&
           ('a' == c || 'e' == c || 'i' == c || 'o' == c || 'u' == c)) {
      j--;

      c = tolower(s[j]);
    }

    if (j < i) {
      gap = i - j;
      i = i + 1;
      while (i <= len) {
        s[i - gap] = s[i];
        i++;
      }

      i = j + 1;
    }

    i--;
    j = i;
  }
}

void removeVowelsFromString2(char s[]) {
  int len;
  int i;
  int j;
  int c;

  len = strlen(s);
  i = 0;
  j = 0;

  while (i < len) {
    c = tolower(s[i]);
    while ('a' == c || 'e' == c || 'i' == c || 'o' == c || 'u' == c) {
      i++;
      c = tolower(s[i]);
    }

    if (i > j) {
      s[j] = s[i];
    }

    i++;
    j++;
  }

  s[j] = '\0';
}

void runRemoveVowelFromStrings(void) {
  char s1[] = "ixxaayaeiouye";
  char s2[] = "ixxaayaeiouye";

  printf("The original string is \"%s\"\n", s1);
  removeVowelsFromString1(s1);
  printf("After moving vowels is \"%s\"\n", s1);

  printf("\n");
  printf("Most efficient in line removing...\n");
  printf("The original string is \"%s\"\n", s2);
  removeVowelsFromString2(s2);
  printf("After moving vowels is \"%s\"\n", s2);
}

/*
 * Test 10:
 *
 * Return the k number of numbers that adds up to maximum value, assuming output
 * is preallocated.
 */
int *maxSubsequence(int *nums, int numsSize, int k, int *returnSize) {
  int outputSize;
  int i;
  int j;
  int smallestIndex;
  int smallestValue;
  int tmp;
  int *output;

  output = malloc(sizeof(int) * k);
  if (NULL == output) {
    return NULL;
  }

  outputSize = 0;
  for (i = 0; i < numsSize; i++) {
    if (outputSize < k) {
      output[outputSize++] = nums[i];
    } else {
      /* output is full, so if there is one in output that is
       * smaller than input[i], we shift the output list to left
       * to free up space at the end and add new input[i] at end.
       */
      smallestValue = nums[i];
      smallestIndex = -1;

      for (j = outputSize - 1; j >= 0; j--) {
        if (output[j] < smallestValue) {
          smallestIndex = j;
          smallestValue = output[j];
        }
      }

      if (smallestIndex >= 0) {
        for (j = smallestIndex; j < outputSize - 1; j++) {
          output[j] = output[j + 1];
        }

        output[outputSize - 1] = nums[i];
      }
    }
  }

  *returnSize = outputSize;

  return output;
}

void runGetMaxNumberOfElements(void) {
  int array1[] = {3, 1, -3, -4, 2, 4, 5};
  int *output;
  int outputSize;
  int i;

  printf("\n");
  printf("Test 10: find k elements added up to max value\n");
  printIntegerArray(array1, sizeof(array1) / sizeof(array1[0]));

  printf("\n");
  printf("2 max elements: ");
  output = maxSubsequence(array1, sizeof(array1) / sizeof(array1[0]), 2,
                          &outputSize);
  printIntegerArray(output, outputSize);
  free(output);

  printf("\n");
  printf("3 max elements: ");
  output = maxSubsequence(array1, sizeof(array1) / sizeof(array1[0]), 3,
                          &outputSize);
  printIntegerArray(output, outputSize);
  free(output);

  printf("\n");
  printf("1 max elements: ");
  output = maxSubsequence(array1, sizeof(array1) / sizeof(array1[0]), 1,
                          &outputSize);
  printIntegerArray(output, outputSize);
  free(output);

  printf("\n");
  printf("6 max elements: ");
  output = maxSubsequence(array1, sizeof(array1) / sizeof(array1[0]), 6,
                          &outputSize);
  printIntegerArray(output, outputSize);
  free(output);

  printf("\n");
}

/*
 * Test 11: return number of valid combination that fills up a matrix
 * evenly.
 */

/* Sum up decimal value of each digit.
 */
int getSumOfDigit(long value) {
  int sum = 0;
  int quotient;

  while (value > 0) {
    /* on the machine I have, divide and then multiple and minus
     * the result from original value is cheaper than % to derive
     * remainder.
     *
     * sum = sum + (value % 10);
     * value = value / 10;
     */

    quotient = value / 10;
    sum = sum + (value - quotient * 10);
    value = quotient;
  }

  return sum;
}

/* Sum up decimal value of each digit
 */
int isSumOfDigitOfValueEqualToNum(long value, int num) {
  int sum = 0;
  int quotient;

  while (value > 0) {
    /* on the machine I have, divide and then multiple and minus
     * the result from original value is cheaper than % to derive
     * remainder.
     *
     * sum = sum + (value % 10);
     * value = value / 10;
     */

    quotient = value / 10;

    sum = sum + (value - quotient * 10);
    if (sum > num) {
      return 0;
    }

    value = quotient;
  }

  return sum == num;
}

/* Return the possible maximum value of the valid combination.
 */
long getMaxValueOfCombination(int numCells, int numColors) {
  int cellPerColor;
  int i;
  int j;
  long maxvalue;

  maxvalue = 0;
  cellPerColor = numCells / numColors;
  for (i = numColors - 1; i >= 0; i--) {
    for (j = 0; j < cellPerColor; j++) {
      maxvalue = (maxvalue * 10) + i;
    }
  }

  return maxvalue;
}

/* Return the possible minimum value of the valid combination.
 */
long getMinValueOfCombination(int numCells, int numColors) {
  int cellPerColor;
  int i;
  int j;
  long maxvalue;

  maxvalue = 0;
  cellPerColor = numCells / numColors;
  for (i = 0; i < numColors; i++) {
    for (j = 0; j < cellPerColor; j++) {
      maxvalue = (maxvalue * 10) + i;
    }
  }

  return maxvalue;
}

/* Return number of valid combinations of matrix (as numCells)
 * and number of colors to fill up the cells evently.
 */
long getNumberOfCombination(int numCells, int numColors) {
  long result;
  long maxvalue;
  long minvalue;
  long i;

  maxvalue = getMaxValueOfCombination(numCells, numColors);
  minvalue = getMinValueOfCombination(numCells, numColors);
  result = 0;

  for (i = minvalue; i < maxvalue; i++) {
    if (isSumOfDigitOfValueEqualToNum(i, numCells)) {
      result++;
    }
  }

  return result;
}

void runGetMaxNumberOfCombinationFillupMatrixEvenly(void) {
  // time_t m;

  // m = time(NULL);

  printf("Test 10: matrix 3 x 3 and 3 colors has %ld combination\n",
         getNumberOfCombination(9, 3));

  printf("Test 10: matrix 4 x 2 and 3 colors has %ld combination\n",
         getNumberOfCombination(8, 2));

  printf("Test 10: matrix 2 x 2 and 2 colors has %ld combination\n",
         getNumberOfCombination(4, 2));

  // printf("Total time %lu\n", time(NULL) - m);

  printf("\n");
}

// the starting point...
int main(int argc, char *argv[]) {
  runFindMissingNumberInTheArray();
  printf("\n");

  runDeteremineIf2NumberSumToValue();
  printf("\n");

  runStringSegmentation();
  printf("\n");

  runFindLowHighIndex();
  printf("\n");

  runFindKLargestElementsInArray();
  printf("\n");

  runRotateArrayByKElement();
  printf("\n");

  runRotateMatrix90DegreeToRight();
  printf("\n");

  runMergeTwoLists();
  printf("\n");

  runRemoveVowelFromStrings();
  printf("\n");

  runGetMaxNumberOfElements();
  printf("\n");

  runGetMaxNumberOfCombinationFillupMatrixEvenly();
  printf("\n");

  return 0;
}
