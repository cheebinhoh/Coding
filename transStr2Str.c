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
            && count < 5000 )
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
    char str1[]   = "EACBD";
    char str2[]   = "CABED";
    char str3[]   = "EACBD";
    char target[] = "EABCD";
    int  moveCnt = 0;


    printf("target string = %s\n\n", target);

    // we do not do printf(...., str1, transform(str1, target), str1 );
    // because comma separator on function arguments are not sequencing point, there
    // is no guarantee that 1st str1 is passed to printf before it is transformed by method transform

    printf("string1 = %s\n", str1);
    moveCnt = transform(str1, target);
    printf("after %d move, string1 = %s\n", moveCnt, str1);

    printf("string2 = %s\n", str2);
    moveCnt = transform(str2, target);
    printf("after %d move, string2 = %s\n", moveCnt, str2);

    return 0;
}
