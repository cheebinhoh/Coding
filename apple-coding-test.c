/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Given an array of integers and a value, determine if there are any three
 * integers in the array whose sum equals the given value.
 */

#include <stdio.h>

/* How to improve the performance? if we keep the loop in sorted, then in the
 * 2nd and 3rd level of loop, we can cut off early if the element k added to
 * j and i already exceed sum, we can skip the rest of k.
 */
int deteremineIf3NumberSumToValue(int array[], int size, int sum)
{
    int i;
    int j;
    int k;

    for ( i = 0; i < size; i++ )
    {
        for ( j = 0; j < size; j++ )
        {
           if ( j == i )
               continue;

           for ( k = 0; k < size; k++ )
           {
               if ( k == j )
                   continue;

               if ( k == i )
                   continue;

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
