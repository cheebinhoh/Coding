/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Coding test questions from Amazon (at least internet claim it :)) and many others.
 *
 * https://www.educative.io/blog/crack-amazon-coding-interview-questions#questions
 * questions that is similar I come across from friend or online.
 *
 * All answers are done by me with some assumption sometimes to make it easy for me :)
 * I do some reading about some topic to refresh my knowledge about certain theory, like
 * AVL before I start coding it.
 *
 * Happy coding!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"
#include "llist.h"


void printIntegerArray(int array[], int size)
{
    int i;


    for ( i = 0; i < size - 1; i++ )
       printf("%d, ", array[i]);

    if ( size >= 1 )
       printf("%d\n", array[size - 1]);
}

/* Test 1:
 *
 * you are given an array of positive numbers from 1 to n, such that all numbers
 * from 1 to n are present except one number x. You have to find x. The input array is not sorted.
 * Look at the below array and give it a try before checking the solution
 */
int findMissingNumberInTheArray(int array[], int size, int n)
{
    int sum;
    int expectedSum;
    int i;


    expectedSum = 0;
    for ( i = 1; i <= n; i++ )
        expectedSum += i;

    sum = 0;
    for ( i = 0; i < size; i++ )
        sum += array[i];

    return ( expectedSum - sum );
}

void runFindMissingNumberInTheArray(void)
{
    int array1[] = { 3, 7, 1, 2, 8, 4, 5 };


    printf("Test 1: find the missing number in the array.\n");
    printf("\n");

    printf("The integer array is ");
    printIntegerArray(array1, sizeof(array1) / sizeof(array1[0]));
    printf("The missing integer is %d\n",
           findMissingNumberInTheArray(array1, sizeof(array1) / sizeof(array1[0]), 8));
    printf("\n");
}


/*
 * Test 2:
 *
 * Given an array of integers and a value, determine if there are any two integers in
 * the array whose sum is equal to the given value. Return true if the sum exists and
 * return false if it does not.
 */

void print2NumberSumToValue(int array[], int size, int val)
{
    int              i;
    int              remaining;
    struct TreeNode *root;
    struct TreeNode *node;
    struct TreeNode *foundList;


    root = NULL;
    for ( i = 0; i < size; i++ )
        root = addTreeNode(root, array[i]);

    node      = NULL;
    foundList = NULL;
    for ( i = 0; i < size; i++ )
    {
         node = findTreeNode(foundList, array[i]);
         if ( NULL != node )
             continue;

         remaining = val - array[i];

         node = findTreeNode(root, remaining);
         if ( NULL != node
              && node->val != array[i])
         {
             foundList = addTreeNode(foundList, array[i]);
             foundList = addTreeNode(foundList, node->val);

             printf("[%d, %d] ", array[i], node->val);
         }
    }

    freeTreeNode(root);
    freeTreeNode(foundList);

    printf("\n");
}

void runDeteremineIf2NumberSumToValue(void)
{
    int array1[] = { 5, 7, 1, 2, 8, 4, 3 };
    int found;


    printf("Test 2: determine if the sum of two integers is equal to the given value.\n");
    printf("\n");

    printf("The integer array is ");
    printIntegerArray(array1, sizeof(array1) / sizeof(array1[0]));
    printf("The list of two numbers added to 10 is ");
    print2NumberSumToValue(array1, sizeof(array1) / sizeof(array1[0]), 10);
    printf("\n");
}

/* Test 3:
 *
 * Given a dictionary of words and a large input string. You have to find out whether the
 * input string can be completely segmented into the words of a given dictionary.
 */

int determineIfStringCanBeSegmented(char *dictionary[], char *s)
{
    int i;
    int len;
    int can;


    for ( i = 0; NULL != dictionary[i]; i++ )
    {
        len = strlen(dictionary[i]);

        if ( strncmp(dictionary[i], s, len ) == 0 )
        {
            if ( '\0' == s[len] )
                return 1;
            else if ( determineIfStringCanBeSegmented(dictionary, s + len) )
                return 1;

            // recursive method can be expensive and depending on the string to be segemented,
            // we can be stack overflow, another way is to maintain backtracking explicitly by
            // storing the previous position in a list or array, so that if next search and
            // match does not work, we backtrack to previous poistion in string to explore
            // into other search.
        }
    }

    return 0;
}

void runStringSegmentation(void)
{
    char *dictionary[] = { "apple", "pear", "pie", NULL };
    char  s1[]         = "applepie";
    char  s2[]         = "applepeer";
    int   i;


    printf("Test 3: given a dictionary of words and a large input string. You have to find out "
            "whether the input string can be completely segmented into the words of a given dictionary.\n");
    printf("\n");

    printf("The dictionary words are: ");
    for ( i = 0; NULL != dictionary[i]; i++ )
        printf("\"%s\" ", dictionary[i]);

    printf("\n");

    if ( determineIfStringCanBeSegmented(dictionary, s1) )
         printf("The string \"%s\" can be segmented\n", s1);
    else
         printf("The string \"%s\" can not be segmented\n", s1);

    if ( determineIfStringCanBeSegmented(dictionary, s2) )
         printf("The string \"%s\" can be segmented\n", s2);
    else
         printf("The string \"%s\" can not be segmented\n", s2);

    printf("\n");
}

/*
 * Test 4:
 *
 * Given a sorted array of integers, return the low and high index of the given key.
 * You must return -1 if the indexes are not found. The array length can be in the
 * millions with many duplicates.
 */

int binarySearch(int list[], int size, int value)
{
    int head   = 0;
    int tail   = size - 1;
    int middle = ( head + tail ) / 2;


    while ( head <= tail
            && list[middle] != value )
    {
       if ( value < list[middle] )
          tail = middle - 1;
       else
          head = middle + 1;

       middle = ( head + tail ) / 2;
    }

    return list[middle] == value ? middle : -1;
}

int findLowHighIndex(int array[], int size, int val, int *low, int *high)
{
    int s;
    int e;
    int mid;
    int foundIndex;
    int startIndex;


    *low       = -1;
    *high      = -1;
    foundIndex = binarySearch(array, size, val);
    if ( -1 == foundIndex )
        return foundIndex;

    // find the low
    s     = 0;
    e     = foundIndex - 1;
    while ( s < e )
    {
        mid = ( e - s ) / 2;
        if ( val <= array[mid] )
            e = mid - 1;
        else
            s = mid + 1;
    }

    *low  = e + 1;

    // fidn the high
    s          = foundIndex + 1;
    e          = size - 1;
    startIndex = s;
    while ( e > s )
    {
        mid = s + ( ( e - s ) / 2 );
        if ( val >= array[mid] )
            s = mid + 1;
        else
            e = mid - 1;
    }

    *high = s - 1;

    return *low;
}

void runFindLowHighIndex(void)
{
    int array[] = { 1, 2, 5, 5, 5, 5, 5, 5, 5, 5, 20 };
    int low;
    int high;


    printf("Test 4: given a sorted array of integers, return the low and high index of the given key. "
           "You must return -1 if the indexes are not found. The array length can be in the "
           "millions with many duplicates.\n");
    printf("\n");

    printf("The integer array is ");
    printIntegerArray(array, sizeof(array) / sizeof(array[0]));
    findLowHighIndex(array, sizeof(array) / sizeof(array[0]), 5, &low, &high);
    if ( -1 != low )
        printf("For value %d, the low index is %d and high index is %d\n", 5, low, high);
}


/*
 * Test 5:
 *
 * find k largest(or smallest) elements in an array
 */
void printKLargestElementsInArray(int array[], int size, int k)
{
    int i;
    int n;
    struct ListNode *start = NULL;


    n = 0;
    for ( i = 0; i < size; i++ )
    {
        if ( n < k )
        {
            addListNode(array[i], &start, NULL);
            n++;
        }
        else if ( array[i] > start->val )
        {
            delListNode(start->val, &start, NULL);
            addListNode(array[i], &start, NULL);
        }
    }

    printf("The largest %d values are :", k);
    printListNode(start);
    freeListNode(start);
}

void runFindKLargestElementsInArray(void)
{
    int array1[] = { 1, 23, 12, 9, 30, 2, 50 };


    printf("Test 7: find k largest(or smallest) elements in an array.\n");
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
void rotateArrayByKElement(int array[], int size, int k)
{
    int              i;
    struct ListNode *start = NULL;
    struct ListNode *tmp;


    for ( i = 0; i < k && i < size; i++ )
        addListNode(array[i], &start, NULL);

    for ( i = 0; i < size - k; i++ )
    {
       array[i] = array[i + k];
    }

    tmp = start;
    while ( NULL != tmp )
    {
        array[i++] = tmp->val;
        tmp = tmp->next;
    }

    freeListNode(start);
}

void runRotateArrayByKElement(void)
{
    int array1[] = { 1, 2, 3, 4, 5, 6, 7 };


    printf("Test 6: rotate array by k elements\n");
    printf("\n");
    printf("The integer array is ");
    printIntegerArray(array1, sizeof(array1) / sizeof(array1[0]));
    rotateArrayByKElement(array1, sizeof(array1) / sizeof(array1[0]), 2);

    printf("\n");
    printf("After rotating by 2, the integer array is ");
    printIntegerArray(array1, sizeof(array1) / sizeof(array1[0]));
}

/*
 * Test 7:
 *
 * Rotate matrix 90" degree to right.
 */
void printArrayInMatrix(int array[], int size, int matrixSize)
{
    int r;
    int c;
    int rIndex;


    for ( r = 0; r < matrixSize; r++ )
    {
        rIndex = r * matrixSize;

        for ( c = 0; c < matrixSize; c++ )
        {
            printf("%2d     ", array[rIndex + c]);
        }

        printf("\n");
    }
}

void rotateMatrix90DegreeToRight(int array[], int size, int matrixSize)
{
    int  i;
    int  j;
    int  k;
    int  l;
    int *buffer;


    buffer = malloc(sizeof(int) * matrixSize);
    if ( NULL == buffer )
        return;

    l = 0;
    for ( i = 0; i < matrixSize; i++ )
    {
        k = size - ( matrixSize - i );
        for ( j = 0; j < matrixSize; j++ )
        {
            buffer[j] = array[k];

            if ( k - j - ( i * matrixSize ) > 0 )
                memmove(&array[l + 1], &array[l], ( k - j - ( i * matrixSize ) ) * sizeof( int ));

            k = k - ( matrixSize - i - 1 );
            l++;
        }

        for ( j = 0; j < matrixSize; j++ )
        {
            array[l - matrixSize + j] = buffer[j];
        }
    }

    free(buffer);
}

void runRotateMatrix90DegreeToRight(void)
{
    int array1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int array2[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };


    printf("Test 5: rotate matrix 90\" degree to right.\n");
    printf("\n");

    printf("3 x 3 matrix:\n");
    printArrayInMatrix(array1, sizeof( array1 ) / sizeof( array1[0] ), 3);
    printf("\n");

    rotateMatrix90DegreeToRight(array1, sizeof( array1 ) / sizeof( array1[0] ), 3 );

    printf("After rotating matrix 90\" degree to right:\n");
    printArrayInMatrix(array1, sizeof( array1 ) / sizeof( array1[0] ), 3);
    printf("\n");


    printf("4 x 4 matrix:\n");
    printArrayInMatrix(array2, sizeof( array2 ) / sizeof( array2[0] ), 4);
    printf("\n");

    rotateMatrix90DegreeToRight(array2, sizeof( array2 ) / sizeof( array2[0] ), 4 );

    printf("After rotating matrix 90\" degree to right:\n");
    printArrayInMatrix(array2, sizeof( array2 ) / sizeof( array2[0] ), 4);
    printf("\n");
}

// the starting point...
int main(int argc, char *argv[])
{
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

    return 0;
}
