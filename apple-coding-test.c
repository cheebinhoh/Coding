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

void runDeteremineIf3NumberSumToValue(void)
{
    int number[] = { 0, 1, 2, 3, 4, 5 };
    int found;


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
    int    i;


    for ( i = 0; i < pData->count; i++ )
    {
        if ( pos == pData->positions[i] )
            pData->values[i] = node->val;
    }
}

int addIntegerAndReturnMedian(int val)
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

    data.values[0] = data.values[1] = 0;
    traverseTreeNodeInOrder(root, calculateMedian, &data);

    return ( data.values[0] + data.values[1] ) / data.count;
}

void runFindMedianOfStreamOfIntegers(void)
{
    printf("Median = %2d after adding  5\n", addIntegerAndReturnMedian(5));
    printf("Median = %2d after adding 15\n", addIntegerAndReturnMedian(15));
    printf("Median = %2d after adding  1\n", addIntegerAndReturnMedian(1));
    printf("Median = %2d after adding  3\n", addIntegerAndReturnMedian(3));
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

    return 0;
}
