/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Replace tab with space, and we can potentially do this in awk than
 * a C program.
 *
 * We assume a tab is equivalent to 8 spaces, but it is not always the
 * case.
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>


int   space2Tab = 8;
char *programName = NULL;


void printHelp(void)
{
     fprintf(stderr, "%s [-h] [-t space]\n", programName);
     fprintf(stderr, "\n");
     fprintf(stderr, " -h       : print this help message\n");
     fprintf(stderr, " -t space : number of space per tab [default is 8 and must be 1 or above]\n");
}

int main(int argc, char *argv[])
{
    int c;
    int startWithNoneSpace = 0;


    programName = basename(argv[0]);

    while ( ( c = getopt(argc, argv, "ht:") ) != -1 )
    {
        switch ( c )
        {
            case 't':
                space2Tab = strtol(optarg, NULL, 0);
                break;

            case '?':
            case 'h':
                printHelp();
                exit(1);
        }
    }

    if ( space2Tab <= 0 )
    {
        printHelp();
        exit(1);
    }

    while ( ( c = getchar() ) != EOF )
    {
        if ( '\t' == c
             && !startWithNoneSpace )
        {
            int i;

            for ( i = 0; i < space2Tab; i++ )
                putchar(' ');
        }
        else
        {
            if ( ! isspace(c ) )
               startWithNoneSpace = 1;
            else if ( '\n' == c )
               startWithNoneSpace = 0;

            putchar(c);
        }
    }

    return 0;
}
