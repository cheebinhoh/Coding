/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Given two strings A and B, the task is to convert A to B if possible.
 * The only operation allowed is to put any character from A and insert it at front.
 * Find if it’s possible to convert the string. If yes, then output minimum no. of
 * operations required for transformation.
 *
 * The problem is posted by a friend according to the following link
 * https://www.geeksforgeeks.org/transform-one-string-to-another-using-minimum-number-of-given-operation
 *
 * I found it interesting enough that I would like to solve it (without looking at any
 * answer :)).
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int   debug = 0;
char *programName = NULL;


/* This method will return further distance of pivot character from
 * the start of the string.
 *
 * E.g. if the string is "abc", and pivot is "b", then answer is 1
 *
 * if string is "abbc", and pivot is "b", then it is 2 as that is
 * further character "b" on "abbc" string if numPivotPrior is 0, else
 * it is 1.
 */
int distanceFromPivot(char string[], char pivot, int numPrecedingPivot)
{
    int i = 0;


    while ( string[i] != '\0' )
    {
        if ( string[i] == pivot )
        {
           if ( numPrecedingPivot > 0 )
           {
               break;
           }
           else
           {
               int hasOther = 0;
               int j        = i + 1;


               while ( string[j] != '\0' )
               {
                   if ( string[j] == pivot )
                   {
                       hasOther = 1;
                   }

                   j++;
               }

               if ( ! hasOther  )
               {
                   break;
               }
           }
        }

        i++;
    }

    return i;
}

/* It returns 1 if source can be translated into target, else 0.
 */
int isTransformable(char source[], char target[])
{
    int i       = 0;
    int j       = 0;
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


    if ( ! isTransformable(source, target) )
    {
        return -1;
    }

    i = 0;
    while ( source[i] != '\0'
            && count < 50000 ) // a cut over to prevent infinite loop due to coding mistake
    {
        if ( source[i] == target[i] )
        {
            i++;
        }
        else
        {
           // So that source[i] != target[i] and we want to decide which of the character after
           // ith position in source that we want to move to the front.

           int j     = i + 1;
           int pivot = i + 1;
           int gap   = 0;
           int tmp;
           int pos;
           int k;
           int numPivotPrior;


           while ( source[j] != '\0' )
           {
               k = i;
               numPivotPrior = 0;

               // the use of numPivotPrior is to calculating how many source[j] is needed after position ith
               // and before position jth
               //
               // the result is used to tell distanceFromPivot if it needs to look further than first source[j]
               // in targrt after (i + 1) th position
               while ( k < j )
               {
                   if ( target[k] == source[j] )
                   {
                       numPivotPrior++;
                   }

                   if ( source[k] == source[j] )
                   {
                       numPivotPrior--;
                   }

                   k++;
               }

               pos = distanceFromPivot(target + i, source[j], numPivotPrior) + i;

               if ( pos < j)
               {
                   if ( ( j - pos ) >= gap )
                   {
                       gap = j - pos;
                       pivot = j;
                   }
               }

               j++;
           }

           if ( debug )
           {
               printf("---- Move %2d, pick %c (%2d) and insert at the front, %s => ",
                      move + 1, source[pivot], pivot, source);
           }

           tmp = source[pivot];
           j   = pivot;

           while ( j > 0 )
           {
               source[j] = source[j - 1];
               j--;
           }

           source[0] = tmp;
           i = 0;

           if ( debug )
           {
               printf("%s\n", source);
           }

           move++;
        } // if ( source[i] == target[i] ) ... else

        count++;
    } //  while ( source[i] != '\0' )

    return move;
}

void printHelp(void)
{
     fprintf(stderr, "%s [-d] [-h]\n", programName);
     fprintf(stderr, "\n");
     fprintf(stderr, " -d                             : debug mode to demonstate the move\n");
     fprintf(stderr, " -h                             : print this help message\n");
     fprintf(stderr, " -s source1,source2,source3,... : list of source strings to be transformed into target\n");
     fprintf(stderr, " -t target                      : target string\n");
     fprintf(stderr, "\n");
     fprintf(stderr, " both -s and -t must be specified together\n");
}

void runTest(char string[], char target[])
{
    int moveCnt;


    // we do not do printf(...., str1, transform(str1, target), str1 );
    // because comma separator on function arguments are not sequencing point, there
    // is no guarantee that 1st str1 is passed to printf before it is transformed by nested method transform

    printf("Transofmr string = %s to %s\n", string, target);
    moveCnt = transform(string, target);
    printf("after %d move, string = %s\n\n", moveCnt, string);
}

void runSampleTest(void)
{
    char str1[]    = "EACBD";
    char str2[]    = "CABED";
    char str3[]    = "EACBD";
    char str4[]    = "ABECD";
    char str5[]    = "ACEBD";
    char str6[]    = "ABCDE";
    char str7[]    = "DEABC";
    char target1[] = "EABCD";
    char str8[]    = "OHH";
    char str9[]    = "HHO";
    char target2[] = "HOH";
    char str10[]   = "HCEE";
    char str11[]   = "HEEC";
    char str12[]   = "ECEH";
    char target3[] = "CHEE";
    char str13[]   = "Crprogamming";
    char str14[]   = "Cproggrammin";
    char target4[] = "Cprogramming";


    runTest(str1, target1);
    runTest(str2, target1);
    runTest(str3, target1);
    runTest(str4, target1);
    runTest(str5, target1);
    runTest(str6, target1);
    runTest(str7, target1);

    runTest(str8, target2);
    runTest(str9, target2);

    runTest(str10, target3);
    runTest(str11, target3);
    runTest(str12, target3);

    runTest(str13, target4);
    runTest(str14, target4);
}


int main(int argc, char *argv[])
{
    int   c;
    char *targetArg = NULL;
    char *sourceArg = NULL;
    char  source[1024]; // maximum hardcode


    programName = argv[0];

    while ( ( c = getopt(argc, argv, "s:t:dh") ) != -1 )
    {
        switch ( c )
        {
            case 'd':
                debug = 1;
                break;

            case 't':
                targetArg = optarg;
                break;

            case 's':
                sourceArg = optarg;
                break;

            case '?':
            case 'h':
                printHelp();
                exit(1);
        }
    }

    if ( NULL != sourceArg
         && NULL == targetArg )
    {
        printHelp();
        exit(1);
    }

    if ( NULL == sourceArg
         && NULL != targetArg )
    {
        printHelp();
        exit(1);
    }

    if ( NULL == sourceArg )
    {
        runSampleTest();
    }
    else
    {
        char *token;


        token = strtok(sourceArg, ",");
        while ( NULL != token )
        {
            strncpy(source, token, sizeof( source ) - 1);
            source[sizeof( source ) - 1] = '\0';

            runTest(source, targetArg);

            token = strtok(NULL, ",");
        }
    }

    return 0;
}
