/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Coding test questions from Apple (at least internet claim it :)).
 *
 * https://www.codinginterview.com/apple-interview-questions, I also include
 * questions that is similar I come across from friend or online.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "btree.h"


/* Test 1:
 *
 * Given an array of integers and a value, determine if there are any three
 * integers in the array whose sum equals the given value.
 *
 * How do we improve the performance? one way is to keep the array in sorted,
 * so we traverse from start (smaller number), if the current value at j is
 * positive value and j + k already exceeds sum, then we can skip the remaining as
 * we are not going to find a sum of i + j + i that is equal to sum.
 */
int deteremineIf3NumberSumToValue(int array[], int size, int sum)
{
    int i;
    int j;
    int k;


    for ( i = 0; i < size; i++ )
    {
        for ( j = i + 1; j < size; j++ )
        {
           for ( k = j + 1; k < size; k++ )
           {
               if ( ( array[i] + array[j] + array[k] ) == sum )
                   goto found;
           }
        }
    }

    return 0;

found:
    return 1;
}

// in the case we have a long list of items, e.g. 5000 and the 3 values added up to the sum are
// located between 4500 to 5000, then searching from the beginning will be in efficient.
//
// in this case, we can make use of binary search approach that we do the following iterations:
// - 1st = midpoint is at 2500, we try to find two values from 0 - 2499 and another two from 2501 - 5000
//   to add up to sum.
// - 2nd = midpoint is at 3751, we try to find two values from 2501 - 3750 and aniother two from 3752 - 5000
// - 3rd = midpoint is at 4377, and we further divide it into two panes.
//
// if we are diving into right pane, we will shorten the array in subcall as we do not awant the subcall
// left pane to extend toward all the way to 0.
int deteremineIf3NumberSumToValueInDivAndConquerRecursive(int array[], int size, int sum, int mid)
{
    int i;
    int j;


    if ( mid >= size )
        return 0;

    if ( mid >= 2 )
    {
        for ( i = mid - 1; i > 0; i-- )
        {
            for ( j = i - 1; j >= 0; j-- )
            {
               if ( ( array[mid] + array[i] + array[j] ) == sum )
                   goto found;
            }
        }
    }

    for ( i = mid + 1; i < size - 1; i++ )
    {
        for ( j = i + 1; j < size; j++ )
        {
            if ( ( array[mid] + array[i] + array[j] ) == sum )
                goto found;
        }
    }

    if ( size - mid >= 5
         && deteremineIf3NumberSumToValueInDivAndConquerRecursive(&array[mid + 1],
                                                                  size - mid - 1,
                                                                  sum,
                                                                  ( size - mid ) / 2 ) )
        goto found;

    if ( mid > 0
         &&  deteremineIf3NumberSumToValueInDivAndConquerRecursive(array, size, sum, mid / 2 ) )
        goto found;

    return 0;

found:
    return 1;
}

int deteremineIf3NumberSumToValueInDivAndConquer(int array[], int size, int sum)
{
    return deteremineIf3NumberSumToValueInDivAndConquerRecursive(array,
                                                                 size,
                                                                 sum,
                                                                 size / 2);
}

void runDeteremineIf3NumberSumToValue(void)
{
    int number[] = { 0, 1, 2, 3, 4, 5 };
    int found;


    printf("Use brute force method\n");
    found = deteremineIf3NumberSumToValue(number,
                                          sizeof( number ) / sizeof( number[0] ),
                                          6);

    if ( found )
    {
       printf("There are 3 numbers sum up to %d\n", 6);
    }
    else
    {
       printf("There are no 3 numbers sum up to %d\n", 6);
    }

    found = deteremineIf3NumberSumToValue(number,
                                          sizeof( number ) / sizeof( number[0] ),
                                          3);

    if ( found )
    {
       printf("There are 3 numbers sum up to %d\n", 3);
    }
    else
    {
       printf("There are no 3 numbers sum up to %d\n", 3);
    }

    found = deteremineIf3NumberSumToValue(number,
                                          sizeof( number ) / sizeof( number[0] ),
                                          22);

    if ( found )
    {
       printf("There are 3 numbers sum up to %d\n", 22);
    }
    else
    {
       printf("There are no 3 numbers sum up to %d\n", 22);
    }

    printf("\n");
    printf("Use Binary way of brute force method\n");
    found = deteremineIf3NumberSumToValueInDivAndConquer(number,
                                                         sizeof( number ) / sizeof( number[0] ),
                                                         6);

    if ( found )
    {
       printf("There are 3 numbers sum up to %d\n", 6);
    }
    else
    {
       printf("There are no 3 numbers sum up to %d\n", 6);
    }

    found = deteremineIf3NumberSumToValueInDivAndConquer(number,
                                                         sizeof( number ) / sizeof( number[0] ),
                                                         3);

    if ( found )
    {
       printf("There are 3 numbers sum up to %d\n", 3);
    }
    else
    {
       printf("There are no 3 numbers sum up to %d\n", 3);
    }

    found = deteremineIf3NumberSumToValueInDivAndConquer(number,
                                                         sizeof( number ) / sizeof( number[0] ),
                                                         22);

    if ( found )
    {
       printf("There are 3 numbers sum up to %d\n", 22);
    }
    else
    {
       printf("There are no 3 numbers sum up to %d\n", 22);
    }
}


/* Test 2:
 *
 * Given a list of intervals, merge all the overlapping intervals to produce a list
 * that has only mutually exclusive intervals.
 *
 * Assumption = it is arranged in ordering of interval start and end.
 */
int collapseOverlapInterval(int start[], int end[], int size)
{
    int i;
    int prev;


    prev = 0;
    i    = 1;
    while ( i < size )
    {
        if ( start[i] >= start[prev]
             && start[i] <= end[prev] )
        {
            if ( end[i] > end[prev] )
            {
                end[prev] = end[i];
            }
        }
        else
        {
            prev++;

            if ( i - prev > 0 )
            {
                start[prev] = start[i];
                end[prev]   = end[i];
            }
        }

        i++;
    }

    return ( size < 2 ) ? size : prev + 1;
}

void printInterval(int start[], int end[], int size)
{
    int i;

    for ( i = 0; i < size - 1; i++ )
    {
        printf("%d...%d, ", start[i], end[i]);
    }

    if ( size > 0 )
        printf("%d...%d\n", start[i], end[i]);
}

void runCollapseOverlapInterval(void)
{
    int start[] = { 1, 4, 5,  9, 10, 14, 17 };
    int end[]   = { 3, 6, 8, 12, 15, 16, 18 };
    int size    = sizeof( start ) / sizeof( start[0] );


    printInterval(start, end, size);
    size = collapseOverlapInterval(start, end, size);
    printf("after collapse\n");
    printInterval(start, end, size);
}


/* Test 3:
 *
 * Search for a given number in a sorted array, with unique elements,
 * that has been rotated by some arbitrary number. Return -1 if the number
 * does not exist.
 */
int binarySearchOnRotateList(int list[], int size, int rotatedStep, int value)
{
    int head   = 0;
    int tail   = size - 1;
    int middle = ( head + tail ) / 2;
    int index;


    while ( head <= tail )
    {
       index = ( rotatedStep + middle ) % size;

       if ( list[index] == value )
       {
          return index;
       }
       else if ( value < list[index] )
       {
          tail = middle - 1;
       }
       else
       {
          head = middle + 1;
       }

       middle = ( head + tail ) / 2;
    }

    return -1;
}

int binarySearch(int list[], int size, int value)
{
    int rotatedStep = 0;
    int prev;
    int i;


    // find out how much has been rotated.
    if ( size > 1 )
    {
        prev = list[0];

        for ( i = 1; i < size; i++ )
        {
            if ( prev > list[i] )
            {
                rotatedStep = i;
                break;
            }

            prev = list[i];
        }
    }

    return binarySearchOnRotateList(list, size, rotatedStep, value);
}

void runBinarySearchOnRotatedSortedList(void)
{
    int array[]        = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19 };
    int arrayRotated[] = { 15, 17, 19, 1, 3, 5, 7, 9, 11, 13 };
    int key;
    int i;


    printf("On none-rotated list\n");
    for ( i = 0; i < sizeof( array ) / sizeof( array[0] ); i++ )
        printf("%d, ", array[i]);

    printf("\n");

    key = binarySearch(array, sizeof( array ) / sizeof( array[0] ), 3 );
    printf("data %2d, key = %d\n", 3, key);

    key = binarySearch(array, sizeof( array ) / sizeof( array[0] ), 7 );
    printf("data %2d, key = %d\n", 7, key);

    key = binarySearch(array, sizeof( array ) / sizeof( array[0] ), 8 );
    printf("data %2d, key = %d\n", 8, key);

    key = binarySearch(array, sizeof( array ) / sizeof( array[0] ), 17 );
    printf("data %2d, key = %d\n", 17, key);


    printf("\n\n");
    printf("On rotated list\n");
    for ( i = 0; i < sizeof( arrayRotated ) / sizeof( arrayRotated[0] ); i++ )
        printf("%d, ", arrayRotated[i]);

    printf("\n");

    key = binarySearch(arrayRotated, sizeof( arrayRotated ) / sizeof( arrayRotated[0] ), 3 );
    printf("data %2d, key = %d\n", 3, key);

    key = binarySearch(arrayRotated, sizeof( arrayRotated ) / sizeof( arrayRotated[0] ), 7 );
    printf("data %2d, key = %d\n", 7, key);

    key = binarySearch(arrayRotated, sizeof( arrayRotated ) / sizeof( arrayRotated[0] ), 8 );
    printf("data %2d, key = %d\n", 8, key);

    key = binarySearch(arrayRotated, sizeof( arrayRotated ) / sizeof( arrayRotated[0] ), 17 );
    printf("data %2d, key = %d\n", 17, key);
}


/* Test 4:
 *
 * Given a positive integer, target, print all possible combinations of positive integers that
 * sum up to the target number.
 *
 * For example, if we are given input ‘5’, these are the possible sum combinations.
 *
 * 4, 1
 * 3, 2
 * 3, 1, 1
 * 2, 3
 * 2, 2, 1
 * 2, 1, 1, 1,
 * 1, 4
 * 1, 3, 1
 * 1, 2, 1, 1
 * 1, 1, 1, 1, 1
 */

void findAllSumCombinationRecursive(int target, int start, int remaining)
{
    int i;


    printf(", %d", remaining);

    i = target - ( start + remaining );
    while ( i-- > 0 )
        printf(", %d", 1);

    remaining--;
    if ( remaining <= 0 )
        return;

    printf("\n");
    printf("%d", start);
    findAllSumCombinationRecursive(target, start, remaining);
}

void findAllSumCombination(int target)
{
    int i;


    i = target - 1;
    while ( i > 0 )
    {
        printf("%d", i);
        findAllSumCombinationRecursive(target, i, target - i);
        printf("\n");

        i--;
    }
}

void runFindAllSumCombination(void)
{
    printf("find all sum combination of 5\n");
    findAllSumCombination(5);
}


/* Test 5:
 *
 * Reverse the order of words in a given sentence (an array of characters).
 *
 *    "123 45 67  89" ==> "89  67 45 123"
 *
 * This is much simple solution of reversing words in a string, we first
 * reverse whole string, the result of that last character is moved to 1st
 * position, 2nd last is moved to 2nd character.
 *
 * The result of it is a string with characters of the each word reversed,
 * then we continue to reverse characters of a word that is broad defintion
 * consists of a consecutive sequence of none-space characters or a consecutive
 * space of space characters. :)
 */
void reverseWords1(char s[])
{
    char *p;
    char *pEnd;
    char  tmp;
    int   swpCount;


    // find the end of the string.
    pEnd = s;
    while ( '\0' != *pEnd )
    {
       pEnd++;
    }

    // reverse characters in the whole string.
    swpCount = ( pEnd - s ) / 2;

    pEnd--;
    p = s;
    while ( swpCount > 0 )
    {
        tmp   = *p;
        *p    = *pEnd;
        *pEnd = tmp;

        p++;
        pEnd--;
        swpCount--;
    }

    // reverse consecutive of none-space characters or consecutive sequence
    // of space characters in each chunk.
    p = s;
    if ( '\0' != *p )
    {
        int   isSpace;
        int   nextIsSpace;
        char *pEndWord;


        pEnd    = p;
        isSpace = isspace(*pEnd);

        while ( '\0' != *pEnd )
        {
            nextIsSpace = isspace(*pEnd);

            if ( isSpace != nextIsSpace )
            {
                pEndWord = pEnd - 1;
                swpCount = ( pEndWord - p + 1 ) / 2;

                while ( swpCount > 0)
                {
                    tmp       = *p;
                    *p        = *pEndWord;
                    *pEndWord = tmp;

                    pEndWord--;
                    p++;
                    swpCount--;
                }

                isSpace = nextIsSpace;
                p       = pEnd;
            }

            pEnd++;
        }

        // There are two ways to remove duplicated code of reversing letters of a word (inside
        // above while loop and after while loop).
        //
        // one way is that we generalize the logic in while loop above so that if we detect
        // an '\0' for next pEnd, we also reverse the letters of the word. I think this is a
        // bad choice as the logic inside the whole loop will become more complicated with trailing
        // conditions.
        //
        // the other way is that we put the logic inside a method or macro (for performance reason),
        // it will remove duplicated code, but it will also mean that anyone reading the code will
        // have to context switch to another method. :)

        pEndWord = pEnd - 1;
        swpCount = ( pEndWord - p + 1 ) / 2;

        while ( swpCount > 0)
        {
            tmp       = *p;
            *p        = *pEndWord;
            *pEndWord = tmp;

            pEndWord--;
            p++;
            swpCount--;
        }
    } // if ( '\0' != *p )
}

void runReverseWords(void)
{
    char str[]  = "1 23 45 6 789";
    char str2[] = " abc  xyz ";


    printf("String = \"%s\"\n", str);
    reverseWords1(str);
    printf("Reversing words, result string = \"%s\"\n", str);

    reverseWords1(str);
    printf("Reversing words again, result string = \"%s\"\n", str);

    printf("\n");
    printf("String = \"%s\"\n", str2);
    reverseWords1(str2);
    printf("Reversing words, result string = \"%s\"\n", str2);

    reverseWords1(str2);
    printf("Reversing words again, result string = \"%s\"\n", str2);
}


/* Test 6:
 *
 * Find total ways to achieve a given sum with `n` throws of dice having `k` faces
 *
 * Example if throw is 2 and sum value is 10, then we have the following:
 * (6, 4), (4, 6), (5, 5)
 */
void printAllWayToThrowDiceEqualToSumRecursive(int sum, int maxThrow, int throwValue[], int throwCnt)
{
    int i;
    int dice;
    int total;


    total = 0;
    for ( i = 0; i < throwCnt; i++ )
    {
        total += throwValue[i];
    }

    if ( throwCnt >= maxThrow )
    {
        if ( total == sum )
        {
            for ( i = 0; i < throwCnt - 1; i++ )
                printf("%d, ", throwValue[i]);

            printf("%d\n", throwValue[i]);
        }

        return;
    }

    dice = 1;
    while ( dice <= 6 )
    {
        if ( ( dice + total ) <= sum )
        {
            throwValue[throwCnt] = dice;

            printAllWayToThrowDiceEqualToSumRecursive(sum, maxThrow, throwValue, throwCnt + 1);
        }

        dice++;
    }
}

void printAllWayToThrowDiceEqualToSum(int sum, int maxThrow)
{
    int throwValue[100];

    printAllWayToThrowDiceEqualToSumRecursive(sum, maxThrow, throwValue, 0);
}

void runAllWayToThrowDiceEqualToSum()
{
    printf("Number of throws = %d, desired sum = %d\n", 2, 10);
    printAllWayToThrowDiceEqualToSum(10, 2);

    printf("\n");
    printf("Number of throws = %d, desired sum = %d\n", 3, 8);
    printAllWayToThrowDiceEqualToSum(8, 3);
}


/* Test 7:
 *
 * You are given a double, x and an integer n, write a function to calculate x raised to the power n.
 *
 *   2 ^  5 = 32
 *   3 ^  4 = 81
 * 1.5 ^  3 = 3.375
 *   2 ^ -2 = 0.25
 */

double power(double value, int power)
{
    double result;
    int    isNegativeExponent;
    int    isNegativeBase;


    isNegativeExponent = 0;
    isNegativeBase = 0;

    if ( value < 0.0 )
    {
        isNegativeBase = 1;
        value = value * -1.0;
    }

    if ( power < 0.0 )
    {
        isNegativeExponent = 1;
        power = power * -1;
    }

    result = value;
    while ( --power > 0 )
        result = result * value;

    if  ( isNegativeExponent )
        result = 1.0 / result;

    if ( isNegativeBase )
        result = -1.0 * result;

    return result;
}

void runCalculatePowerOfDoubleValue(void)
{
    printf("  2 ^  5 = %lf\n", power(2.0, 5));
    printf("  3 ^  4 = %lf\n", power(3.0, 4));
    printf("1.5 ^  3 = %lf\n", power(1.5, 3));
    printf("  2 ^ -2 = %lf\n", power(2.0, -2));
}

/* Test 8:
 *
 * Given that integers are read from a data stream. Find median of elements
 * read so for in efficient way. For simplicity assume there are no duplicates.
 */
struct medianData
{
    int count;
    int positions[2];
    int values[2];
};

void calculateMedian(struct TreeNode *node, int pos, void *data)
{
    struct medianData *pData = data;
    int                i;


    for ( i = 0; i < pData->count; i++ )
    {
        if ( pos == pData->positions[i] )
            pData->values[i] = node->val;
    }
}

double addIntegerAndReturnMedian(int val)
{
    static struct TreeNode *root = NULL;
    struct medianData       data;
    int                     count;


    root = addTreeNodeAndRebalanceTree(root, val);

    count = findTotalNumberOfTreeNode(root);
    if ( ( count % 2 ) == 0 )
    {
        data.count        = 2;
        data.positions[0] = ( count / 2 ) - 1;
        data.positions[1] = data.positions[0] + 1;
    }
    else
    {
        data.count        = 1;
        data.positions[0] = count / 2;
    }

    data.values[0] = 0;
    data.values[1] = 0;
    traverseTreeNodeInOrder(root, calculateMedian, &data);

    return ( data.values[0] + data.values[1] ) / (double) data.count;
}

void runFindMedianOfStreamOfIntegers(void)
{
    printf("Adding  5, median = %5.2f\n", addIntegerAndReturnMedian(5));
    printf("Adding 15, median = %5.2f\n", addIntegerAndReturnMedian(15));
    printf("Adding  1, median = %5.2f\n", addIntegerAndReturnMedian(1));
    printf("Adding  3, median = %5.2f\n", addIntegerAndReturnMedian(3));
    printf("Adding  2, median = %5.2f\n", addIntegerAndReturnMedian(2));
    printf("Adding  4, median = %5.2f\n", addIntegerAndReturnMedian(4));
}

/* Test 9:
 *
 * the permutations of string ABC is "ABC ACB BAC BCA CBA CAB"
 */
void printPermutationsOfString(char s[])
{
    int  len;
    int  i;
    int  j;
    char tmp;


    len = strlen(s);
    for ( i = 0; i < len; i++ )
    {
        printf("%s\n", s);

        for ( j = 1; j < len - i; j++)
        {
            tmp = s[j];
            s[j] = s[0];
            s[0] = tmp;

            printf("%s\n", s);

            tmp = s[j];
            s[j] = s[0];
            s[0] = tmp;
        }

        for ( j = 0; j < len - 1; j++ )
        {
            tmp = s[j];
            s[j] = s[j + 1];
            s[j + 1] = tmp;
        }
    }
}

void runPermutationsOfString(void)
{
    char s[] = "abc";


    printf("Print permutations of string \"abc\"\n");
    printPermutationsOfString(s);
    printf("\n");
}

/* Test 10:
 *
 * AVL self-balancing tree
 */

void runAVLSelfbalanceTree(void)
{
    struct TreeNode *root = NULL;

    root = addTreeNodeAndRebalanceTree(root, 1);
    root = addTreeNodeAndRebalanceTree(root, 2);
    root = addTreeNodeAndRebalanceTree(root, 3);
    root = addTreeNodeAndRebalanceTree(root, 4);
    root = addTreeNodeAndRebalanceTree(root, 5);
    root = addTreeNodeAndRebalanceTree(root, 6);
    root = addTreeNodeAndRebalanceTree(root, 7);
    root = addTreeNodeAndRebalanceTree(root, 8);
    root = addTreeNodeAndRebalanceTree(root, 9);
    root = addTreeNodeAndRebalanceTree(root, 10);
    root = addTreeNodeAndRebalanceTree(root, 11);
    root = addTreeNodeAndRebalanceTree(root, 12);
    root = addTreeNodeAndRebalanceTree(root, 13);
    printTreeNodeInTreeTopology(root);
}

/* Test 11:
 *
 * The maximum subarray problem is the task of finding the largest possible sum of a
 * contiguous subarray, within a given one-dimensional array A[1…n] of numbers
 */

void printMaximumSubarrayNumber(int array[], int size)
{
    int i;
    int j;
    int start;
    int end;
    int val;
    int largest;
    int pivotStart;
    int pivotEnd;
    int pivotLargest;


    if ( size <= 0 )
        return;

    pivotStart = -1;
    pivotEnd   = -1;

    for ( i = 0; i < size; i++ )
    {
        start   = i;
        end     = i;
        val     = array[i];
        largest =  val;


        for ( j = i + 1; j < size; j++ )
        {
            val = val + array[j];

            if ( val >= largest )
            {
                largest = val;
                end     = j;
            }
        }

        if ( -1 == pivotEnd
             || largest > pivotLargest )
        {
            pivotStart   = start;
            pivotEnd     = end;
            pivotLargest = largest;
        }
    }

    for ( i = pivotStart; i <= pivotEnd; i++ )
        printf("%d, ", array[i]);

    printf("\n");
}

void runMaximumSubarrayNumber(void)
{
    int array[] = { -2, 1, -3, 4, -1, 2, 1, -5, 4 };
    int i;


    for ( i = 0; i < ( sizeof( array ) / sizeof( array[0] ) ); i++ )
        printf("%d, ", array[i]);

    printf("\n\n");
    printf("Maximum subarray is ");
    printMaximumSubarrayNumber(array, sizeof( array ) / sizeof( array[0] ) );
}

/* Test 12:
 *
 * Find the rotating point of an array that is sorted, but the starting point can be
 * in middle.
 */

int findRotatingPointInArray(int array[], int size)
{
    int start;
    int end;
    int rotatingPoint;
    int midpoint;


    start         = 0;
    end           = size - 1;
    rotatingPoint = 0;

    while ( start < end
            && array[start] > array[end] )
    {
        midpoint = ( start + end ) / 2;

        if ( array[start] > array[midpoint] )
        {
            end           = midpoint - 1;
            rotatingPoint = end;
        }
        else
        {
            start         = midpoint + 1;
            rotatingPoint = start;
        }
    }

    return rotatingPoint;
}

void findRotatingPointInIntegerArray(void)
{
    int array1[] = { 5, 1, 2, 3, 4 };
    int array2[] = { 4, 5, 6, 1, 2, 3 };
    int array3[] = { 4, 5, 6, 7, 8, 0, 1, 2, 3 };
    int i;
    int start;


    for ( i = 0; i < sizeof( array1 ) / sizeof( array1[0] ); i++ )
        printf("%d, ", array1[i]);

    printf("\n");
    start = findRotatingPointInArray(array1, sizeof( array1 ) / sizeof( array1[0] ));
    printf("The rotating point is at index %d and value is %d\n",
            start, array1[start]);

    printf("\n");
    for ( i = 0; i < sizeof( array2 ) / sizeof( array2[0] ); i++ )
        printf("%d, ", array2[i]);

    printf("\n");
    start = findRotatingPointInArray(array2, sizeof( array2 ) / sizeof( array2[0] ));
    printf("The rotating point is at index %d and value is %d\n",
            start, array2[start]);

    printf("\n");
    for ( i = 0; i < sizeof( array3 ) / sizeof( array3[0] ); i++ )
        printf("%d, ", array3[i]);

    printf("\n");
    start = findRotatingPointInArray(array3, sizeof( array3 ) / sizeof( array3[0] ));
    printf("The rotating point is at index %d and value is %d\n",
            start, array3[start]);
}

/* Test 13:
 *
 * Check if a tree is binary search tree.
 */

struct checkBinarySearchTreeData
{
    int isValid;
    int lastValue;
    int started;
};

void checkTreeNodeValueInOrder(struct TreeNode *node, int pos, void *data)
{
     struct checkBinarySearchTreeData *pData;


     pData = data;
     if ( ! pData->started )
          pData->started = 1;
     else if ( node->val  < pData->lastValue )
         pData->isValid = 0;

     pData->lastValue = node->val;
}


int isBinarySearchTree(struct TreeNode *root)
{
    struct checkBinarySearchTreeData data;


    if ( NULL == root )
        return 1;

    data.lastValue = 0;
    data.isValid   = 1;
    data.started   = 0;

    traverseTreeNodeInOrder(root, checkTreeNodeValueInOrder, &data);

    return data.isValid;
}


void runCheckIfIsBinarySearchTree(void)
{
    struct TreeNode *root1 = NULL;
    struct TreeNode *root2 = NULL;


    root1 = addTreeNodeAndRebalanceTree(root1, 1);
    root1 = addTreeNodeAndRebalanceTree(root1, 2);
    root1 = addTreeNodeAndRebalanceTree(root1, 3);
    root1 = addTreeNodeAndRebalanceTree(root1, 4);

    printTreeNodeInTreeTopology(root1);
    printf("\n");

    if ( isBinarySearchTree(root1) )
        printf("root1 is a binary search tree\n");
    else
        printf("root1 is not a binary search tree\n");

    root2      = malloc( sizeof( struct TreeNode ) );
    root2->val = 4;

    root2->right        = malloc( sizeof( struct TreeNode ) );
    root2->right->val   = 5;
    root2->right->left  = NULL;
    root2->right->right = NULL;

    root2->left      = malloc( sizeof( struct TreeNode ) );
    root2->left->val = 2;

    root2->left->left = NULL;
    root2->left->right = malloc( sizeof( struct TreeNode ) );

    root2->left->right->val   = 6;
    root2->left->right->left  = NULL;
    root2->left->right->right = NULL;

    printTreeNodeInTreeTopology(root1);
    printf("\n");
    if ( isBinarySearchTree(root2) )
        printf("root2 is a binary search tree\n");
    else
        printf("root2 is not a binary search tree\n");

}


/* Test 14:
 *
 * Check that two lists are combined into a final list in a 1st come 1st serve ordering
 */
int checkIfListAreInOrderOfAnotherList(int list1[], int list1Size,
                                       int list2[], int list2Size)
{
    int i;
    int j;
    int last;


    last = 0;
    for ( i = 0; i < list1Size; i++ )
    {
        if ( last > i )
            j = last + 1;
        else
            j = i;

        while ( j < list2Size )
        {
            if ( list2[j] == list1[i] )
                break;

            j++;
        }

        if ( j == list2Size )
            goto fail;

        last = j;
    }

    return 1;

fail:
    return 0;
}

int checkIfTwoListsAre1stCome1stServe(int list1[], int list1Size,
                                      int list2[], int list2Size,
                                      int list3[], int list3Size)
{
    return checkIfListAreInOrderOfAnotherList(list1, list1Size, list3, list3Size)
           && checkIfListAreInOrderOfAnotherList(list2, list2Size, list3, list3Size);

}

void runCheckIfTwoListsAre1stCome1stServe(void)
{
    int list1[] = { 17,  8, 24 };
    int list2[] = { 12, 19,  2 };
    int list3[] = { 17,  8, 12, 19, 24,  2 };
    int list4[] = {  1,  3,  5 };
    int list5[] = {  2,  4,  6 };
    int list6[] = {  1, 2, 4, 6, 5, 3 };
    int i;
    int inOrder;


    printf("list1 = ");
    for ( i = 0; i < sizeof( list1 ) / sizeof( list1[0] ); i++ )
        printf("%d, ", list1[i]);

    printf("\n");
    printf("list2 = ");
    for ( i = 0; i < sizeof( list2 ) / sizeof( list2[0] ); i++ )
        printf("%d, ", list2[i]);

    printf("\n");
    printf("list3 = ");
    for ( i = 0; i < sizeof( list3 ) / sizeof( list3[0] ); i++ )
        printf("%d, ", list3[i]);

    printf("\n");

    inOrder = checkIfTwoListsAre1stCome1stServe(list1, sizeof( list1 ) / sizeof( list1[0] ),
                                                list2, sizeof( list2 ) / sizeof( list2[0] ),
                                                list3, sizeof( list3 ) / sizeof( list3[0] ) );

    if ( inOrder )
        printf("list1 and list2 are served in order\n");
    else
        printf("list1 and list2 are not served in order\n");

    printf("\n");
    printf("list4 = ");
    for ( i = 0; i < sizeof( list4 ) / sizeof( list4[0] ); i++ )
        printf("%d, ", list4[i]);

    printf("\n");
    printf("list5 = ");
    for ( i = 0; i < sizeof( list5 ) / sizeof( list5[0] ); i++ )
        printf("%d, ", list5[i]);

    printf("\n");
    printf("list3 = ");
    for ( i = 0; i < sizeof( list6 ) / sizeof( list6[0] ); i++ )
        printf("%d, ", list6[i]);

    printf("\n");

    inOrder = checkIfTwoListsAre1stCome1stServe(list4, sizeof( list4 ) / sizeof( list4[0] ),
                                                list5, sizeof( list5 ) / sizeof( list5[0] ),
                                                list6, sizeof( list6 ) / sizeof( list6[0] ) );

    if ( inOrder )
        printf("list4 and list5 are served in order\n");
    else
        printf("list4 and list5 are not served in order\n");
}


int main(int argc, char *argv[])
{
    printf("run runDeteremineIf3NumberSumToValue:\n");
    runDeteremineIf3NumberSumToValue();

    printf("\n");
    printf("run runCollapseOverlapInterval:\n");
    runCollapseOverlapInterval();

    printf("\n");
    printf("run runBinarySearchOnRotatedSortedList\n");
    runBinarySearchOnRotatedSortedList();

    printf("\n");
    printf("run runFindAllSumCombination\n");
    runFindAllSumCombination();

    printf("\n");
    printf("run reverseWords\n");
    runReverseWords();

    printf("\n");
    printf("Run runAllWayToThrowDiceEqualToSum\n");
    runAllWayToThrowDiceEqualToSum();

    printf("\n");
    printf("Run runCalculatePowerOfDoubleValue\n");
    runCalculatePowerOfDoubleValue();

    printf("\n");
    printf("Run runFindMedianOfStreamOfIntegers\n");
    runFindMedianOfStreamOfIntegers();

    printf("\n");
    printf("Run runPermutationsOfString\n");
    runPermutationsOfString();

    printf("\n");
    printf("Run runAVLSelfbalanceTree\n");
    runAVLSelfbalanceTree();

    printf("\n");
    printf("Run runMaximumSubarrayNumber\n");
    runMaximumSubarrayNumber();

    printf("\n");
    printf("Run findRotatingPointInIntegerArray\n");
    findRotatingPointInIntegerArray();

    printf("\n");
    printf("Run runCheckIfIsBinarySearchTree\n");
    runCheckIfIsBinarySearchTree();

    printf("\n");
    printf("Run runCheckIfTwoListsAre1stCome1stServe\n");
    runCheckIfTwoListsAre1stCome1stServe();
    printf("\n");

    return 0;
}
