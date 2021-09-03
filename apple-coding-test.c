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

void runDeteremineIf3NumberSumToValue()
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

int main(int argc, char *argv[])
{
    runDeteremineIf3NumberSumToValue();

    return 0;
}
