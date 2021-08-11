/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * A friend shares above program via the following link
 * https://www.geeksforgeeks.org/transform-one-string-to-another-using-minimum-number-of-given-operation
 *
 * Given two strings A and B, the task is to convert A to B if possible.
 * The only operation allowed is to put any character from A and insert it at front.
 * Find if it’s possible to convert the string. If yes, then shuffle the string A into B
 * in minimum move.
 *
 * I found it interesting enough that I would like to solve it, here is my answer to it.
 *
 * Howver, my program doees not transform the string in minimum move in all cases, the logic only
 * look one move ahead at time and find the best move  among characters to move to front, it does not
 * do backtracking or consider more than one consecutive moves at a time.
 *
 * For example, the following takes 5 moves to transform baxxba to abxxab
 * - abxxba
 * - xabxba
 * - xxabba
 * - bxxaba
 * - abxxab
 *
 * However, this program takes 7 moves:
 * - abaxxb
 * - babaxx (*)
 * - ababxx
 * - xababx
 * - xxabab
 * - bxxaba
 * - abxxab
 *
 * the cause is on 2nd move that it picks last b over any of the two x because all 3 picks (x, x and b)
 * has the same score, and the logic always pick the highest score characters, if multiple characters
 * have same highest score, we always pick the right most characters. This is important as we will move
 * the selected character to the left (front), so always pick the highest score character at the right
 * most will make sure that we are not stuck in infinite local search.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int   debug = 0;
char *programName = NULL;


/* It returns 1 if source can be translated into target, else 0.
 */
int isTransformable(char source[], char target[])
{
    int i       = 0;
    int j       = 0;
    int sH[256] = { 0 };
    int tH[256] = { 0 };


    while ( '\0' != source[i] )
    {
        sH[source[i]]++;

        i++;
    }

    while ( '\0' != target[j] )
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

int simulateMoveAndScore(char source[], char target[], int pivot)
{
    int m;
    int n;
    int score;
    int tmp;


    score = 0;
    tmp   = source[pivot];

    // move pivot (jth) character to front, construct new string
    memmove(source + 1, source, pivot);
    source[0] = tmp;

    // Scoring it, the logic is that:
    // - if source and target characters at m and n are matched, plus 1
    // - if they are not matched, we fast forward target to find next nth in
    //   target that matches the source.
    // ---- if next nth matching target is found, then we reset the score to 0.
    // ---- if it is not found, we skip the m and n, and compare next set of
    //      characters.

    n = 0;
    m = 0;
    while ( '\0' != source[m]
            && '\0' != target[n] )
    {
        if ( source[m] == target[n] )
        {
            score++;
            n++;
            m++;
        }
        else
        {
            char *c;


            c = strchr(target + n, source[m]);

            if ( NULL == c )
            {
                m++;
            }
            else
            {
                score = 0;
                n     = c - target;
            }
        }
    }

    // restore original string
    memmove(source, source + 1, pivot);
    source[pivot] = tmp;

    return score;
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
    while ( '\0' != source[i] && count < 50000 ) // a cut over to prevent infinite loop due to coding mistake
    {
        if ( source[i] == target[i] )
        {
            i++;
        }
        else
        {
            int pivot        = i + 1;
            int highestScore = 0;
            int j            = i + 1;
            int score;
            int tmp;


            while ( '\0' != source[j] )
            {
                score = simulateMoveAndScore(source, target, j);

                if ( debug && 0 )
                {
                    printf("---- j = %d, source = %c, score = %d, highestScore = %d\n",
                           j, source[j], score, highestScore);
                }

                if ( score >= highestScore )
                {
                    pivot        = j;
                    highestScore = score;
                }

                j++;
            } /* while ( '\0' != source[j] ) */

            move++;

            if ( debug )
            {
                printf("---- Move %2d, pick %c and insert at the front, %.*s[%.c]%s => ",
                       move, source[pivot], 
                       pivot, source, source[pivot], source + pivot + 1);
            }


            tmp = source[pivot];
            memmove(source + 1, source, pivot);
            source[0] = tmp;

            i = 0;


            if ( debug )
            {
                printf("%s\n", source);
            }
        } // if ( source[i] == target[i] ) ... else

        count++;
    } //  while ( '\0' != source[i] && count < 50000 )

    return move;
}

void printHelp(void)
{
     fprintf(stderr, "%s [-d] [-h] [-s source1,source2,..] [-t target]\n", programName);
     fprintf(stderr, "\n");
     fprintf(stderr, " -d                     : debug mode to demonstate each step of the moves\n");
     fprintf(stderr, " -h                     : print this help message\n");
     fprintf(stderr, " -s source1,source2,... : list of source strings to be transformed into target\n");
     fprintf(stderr, " -t target              : target string\n");
     fprintf(stderr, "\n");
     fprintf(stderr, " if -t is specified, then source string will be from -s argument or standard input (if -s is not specified)\n");
}

void runTest(char string[], char target[])
{
    int moveCnt;


    // we do not do printf(...., str1, transform(str1, target), str1 );
    // because comma separator on function arguments are not sequencing point, there
    // is no guarantee that 1st str1 is passed to printf before it is transformed by nested method transform

    printf("---- Transform %s => %s\n", string, target);

    moveCnt = transform(string, target);

    printf("---- Total move = %2d, string = %s\n\n", moveCnt, string);
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
    char str15[]   = "Hlelo";
    char str16[]   = "Helol";
    char target5[] = "Hello";
    char str17[]   = "ACCBB";
    char target6[] = "ABCBC";

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

    runTest(str15, target5);
    runTest(str16, target5);

    runTest(str17, target6);
}


int main(int argc, char *argv[])
{
    int   c;
    char *targetArg = NULL;
    char *sourceArg = NULL;
    char  source[1024]; // maximum hardcode


    programName = argv[0];

    while ( -1 != ( c = getopt(argc, argv, "s:t:dh") ) )
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

    debug = 1;
    if ( NULL == targetArg )
    {
        runSampleTest();
    }
    else if ( NULL != sourceArg )
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
    else
    {
        while ( fgets(source, sizeof( source ) - 1, stdin) != NULL )
        {
            int len = strlen(source);


            if ( '\n' == source[len - 1] )
            {
                source[len - 1] = '\0';
            }

            runTest(source, targetArg);
        }
    }

    return 0;
}
