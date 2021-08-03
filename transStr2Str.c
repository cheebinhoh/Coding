/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Given two strings A and B, the task is to convert A to B if possible. 
 * The only operation allowed is to put any character from A and insert it at front. 
 * Find if it’s possible to convert the string. If yes, then output minimum no. of 
 * operations required for transformation.
 */

#include <stdio.h>

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
    int i;
    int move = 0;


    // we assume that they are transformable
    i = 0;
    while ( source[i] != '\0' )
    {
        if ( source[i] == target[i] )
        {
            i++;
        }
        else
        {
            int tmp;
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
    }

    return move;
}

int main(int argc, char *argv[])
{
    char str1[] = "EACBD";
    char str2[] = "EABCD";
    char str3[] = "EABCD";
    int  moveCnt = 0;


    printf("target string = %s\n\n", str2);

    printf("string1 = %s\n", str1);
    moveCnt = transform(str1, str2);
    printf("after %d move, string1 = %s\n", moveCnt, str1);

    printf("string3 = %s\n", str3);
    moveCnt = transform(str3, str2);
    printf("after %d move, string1 = %s\n", moveCnt, str3);

    return 0;
}
