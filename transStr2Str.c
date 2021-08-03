/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Given two strings A and B, the task is to convert A to B if possible. 
 * The only operation allowed is to put any character from A and insert it at front. 
 * Find if it’s possible to convert the string. If yes, then output minimum no. of 
 * operations required for transformation.
 *
 * This is work in progress, I got the logic right, but it is not in minimum move yet.
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
    int count = 0;

    // we assume that they are transformable
    i = 0;
    while ( source[i] != '\0'
            && count < 10 )
    {
        if ( source[i] == target[i] )
        {
            i++;
        }
        else
        {
           int j = i;
           int pos;
           int tmp;

           while ( source[j] != '\0' )
           {
               pos = distanceFromPivot( target + i, source[j]) + i;
               if ( pos < j)
                   break;

               j++;
           }
 
           tmp = source[j];
           while ( j > 0 )
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
    int  moveCnt = 0;


    printf("target string = %s\n\n", str2);

    printf("string1 = %s\n", str1);
    moveCnt = transform(str1, str2);
    printf("after %d move, string1 = %s\n", moveCnt, str1);

    return 0;
}
