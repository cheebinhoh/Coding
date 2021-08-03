/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>


int distanceFromPivot(char string[], char pivot)
{
    int i = 0;

    while ( string[i] != '\0'
            && string[i] != pivot )
    {
        i++;
    }

    return i;
}

int transform(char source[], char target[])
{
    int count;
    int i;
    int move = 0;


    // we assume that they are transformable
    i = 0;
    while ( source[i] != '\0'
            && count < 5000 )
    {
        if ( source[i] == target[i] )
        {
            i++;
        }
        else
        {
            int tmp;
            int matchLen = 0;
            int sourceDistance = distanceFromPivot( source, target[i] );
            int targetDistance = distanceFromPivot( target, source[i] );
            int j = i + 1;

            if ( targetDistance < sourceDistance )
            {
                j = i;
                i = i + targetDistance;
            }

            while ( source[j] != '\0'
                    && target[i] != '\0'
                    && source[j] == target[i] )
            {
                j++;
                i++;
            }

            if ( targetDistance >= sourceDistance )
            {
                j = j - 1;
            }

            tmp = source[j];
            while ( j >= 1 )
            {
                source[j] = source[j - 1];
                j--;
            }

            source[0] = tmp;

            i = 0;

            move++;
        }

        count++;
    }

    return move;
}

int main(int argc, char *argv[])
{
    char str1[] = "EACBD";
    char str2[] = "EABCD";
    char str3[] = "EACBD";
    int  moveCnt = 0;

    printf("string1 = %s\n", str1);
    printf("string2 = %s\n", str2);

    moveCnt = transform(str1, str2);

    printf("after %d move, string1 = %s\n", moveCnt, str1);

    return 0;
}
