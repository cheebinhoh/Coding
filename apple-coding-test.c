/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Coding test questions from Apple (at least internet claim it :)).
 */

#include <stdio.h>


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

int main(int argc, char *argv[])
{
    printf("run runDeteremineIf3NumberSumToValue:\n");
    runDeteremineIf3NumberSumToValue();

    printf("\n");
    printf("run runCollapseOverlapInterval:\n");
    runCollapseOverlapInterval();

    return 0;
}
