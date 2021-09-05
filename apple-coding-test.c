/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Coding test questions from Apple (at least internet claim it :)).
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>


/*
 * Given an array of integers and a value, determine if there are any three
 * integers in the array whose sum equals the given value.
 *
 * How do we improve the performance? one way is to keep the array in sorted,
 * so we traverse from start (smaller number), if the current value at j is
 * positive value and j + k already exceeds sum, thn we skip the remaining as
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


/*
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

    for ( i = 0; i < size; i++ )
    {
        printf("%d...%d%s", start[i], end[i],
               ( i + 1 == size ) ? "\n" : ", ");
    }
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


/*
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

       //printf("debug: middle = %d, index = %d, index data = %d, data = %d\n",
       //       middle, index, list[index], value);

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

/*
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

/*
 * Reverse the order of words in a given sentence (an array of characters).
 *
 *    "123 45 67  89" => "89  67 45 123"
 *
 *    >> "123 45 67 89 "
 *    >> "123 45 6789  "
 *    >> "89  123 45 67"
 *
 *    -> "89  123 45 67"
 *    -> "89  67 123 45"
 *    -> "89  67 45 123"
 */

void reverseWords(char s[])
{
    char *p;
    char *start;
    char  tmp;
    int   total;
    int   wordCnt;
    int   wordLen;
    int   prefixSpaceLen;
    int   i;
    int   inWord;


    // count the words
    p = s;
    wordCnt = 0;
    while ( isspace(*p) )
        p++;

    if ( '\0' != *p )
    {
        wordCnt++;
        inWord = 1;


        while ( '\0' != *p )
        {
            if ( isspace(*p) )
            {
                inWord = 0;
            }
            else
            {
                if ( ! inWord )
                {
                    wordCnt++;
                }

                inWord = 1;
            }

            p++;
        }
    }

   start = s;
   p--;
   while ( wordCnt > 1 )
   {
       wordLen = 0;
       while ( ! isspace(*p) )
       {
           wordLen++;
           p--;
       }

       prefixSpaceLen = 0;
       while ( isspace(*p) )
       {
           prefixSpaceLen++;
           p--;
       }

       p++;
       wordLen += prefixSpaceLen;

       // Sometimes I wouder if we should optimize the calculation of wordLen - 1 by precalculating it
       // prior to enter the loop but moderm compiler is good in constant propagation.
       while ( prefixSpaceLen-- > 0 )
       {
           tmp = *p;

           memmove(p, p + 1, wordLen - 1);

           *(p + wordLen - 1) = tmp;
       }

       p = p + (wordLen - 1);
       i = 0;
       while ( i < wordLen )
       {
           tmp = *p;

           memmove(start + 1, start, ( p - start ));
           *start = tmp;

           i++;
       }

       start = start + wordLen;
       wordCnt--;
   }
}

void runReverseWords(void)
{
    char str[] = "123 45 67  89";


    printf("String = %s\n", str);
    reverseWords(str);
    printf("Reverse word, string = %s\n", str);
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

    return 0;
}
