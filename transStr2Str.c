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

int isTransformable(char source[], char target[])
{
    int i = 0;
    int j = 0;
    int sH[256] = { 0 };
    int tH[256] = { 0 };

    while ( source[i] != '\0' )
    {
        sH[source[i]]++;

        i++;
    }

    while ( target[j] != '\0' )
    {
        tH[target[j]]++;

        j++;
    }

    if ( j != i )
    {
        return 0;
    }

    for ( i = 0; i < ( sizeof( sH ) / sizeof( sH[0] ) ); i++ )
    {
        if ( sH[i] != tH[i] )
        {
            return 0;
        }
    }

    return 1;
}

int transform(char source[], char target[])
{
    int i;
    int move  = 0;
    int count = 0;

    // we assume that they are transformable
    if ( ! isTransformable(source, target) )
    {
        return -1;
    }

    i = 0;
    while ( source[i] != '\0' )
    {
        if ( source[i] == target[i] )
        {
            i++;
        }
        else
        {
           int j     = i;
           int pivot = 0;
           int pos;
           int tmp;

           // we want to move the furthest source[j] that its counterpart on target is
           // on left side of the source[j].
           while ( source[j] != '\0' )
           {
               pos = distanceFromPivot(target + i, source[j]) + i;
               if ( pos < j)
               {
                   pivot = j;
               }

               j++;
           }

           j   = pivot;
           tmp = source[j];
           while ( j > 0 )
           {
               source[j] = source[j - 1];
               j--;
           }

           source[0] = tmp;
           i = 0;

           move++;
        } // if ( source[i] == target[i] ) ... else
    } //  while ( source[i] != '\0' )

    return move;
}

int main(int argc, char *argv[])
{
    char str1[]   = "EACBD";
    char str2[]   = "CABED";
    char str3[]   = "EACBD";
    char str4[]   = "ABECD";
    char str5[]   = "ACEBD";
    char str6[]   = "ABCDE";
    char str7[]   = "DEABC";
    char target[] = "EABCD";
    int  moveCnt = 0;


    printf("target string = %s\n\n", target);

    // we do not do printf(...., str1, transform(str1, target), str1 );
    // because comma separator on function arguments are not sequencing point, there
    // is no guarantee that 1st str1 is passed to printf before it is transformed by nested method transform

    printf("string1 = %s\n", str1);
    moveCnt = transform(str1, target);
    printf("after %d move, string1 = %s\n", moveCnt, str1);

    printf("string2 = %s\n", str2);
    moveCnt = transform(str2, target);
    printf("after %d move, string2 = %s\n", moveCnt, str2);

    printf("string3 = %s\n", str3);
    moveCnt = transform(str3, target);
    printf("after %d move, string3 = %s\n", moveCnt, str3);

    printf("string4 = %s\n", str4);
    moveCnt = transform(str4, target);
    printf("after %d move, string4 = %s\n", moveCnt, str4);

    printf("string5 = %s\n", str5);
    moveCnt = transform(str5, target);
    printf("after %d move, string5 = %s\n", moveCnt, str5);

    printf("string6 = %s\n", str6);
    moveCnt = transform(str6, target);
    printf("after %d move, string6 = %s\n", moveCnt, str6);

    printf("string7 = %s\n", str7);
    moveCnt = transform(str7, target);
    printf("after %d move, string7 = %s\n", moveCnt, str7);

    return 0;
}
