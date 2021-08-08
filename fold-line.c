/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * It splits a long lines into multiple line according to max limit per line.
 */


#include <stdio.h>
#include <libgen.h>
#include <unistd.h>
#include <stdlib.h>


#define MAXLINE    80
#define BUFFERSIZE 1024

char *programName = NULL;


void printHelp(void)
{
     fprintf(stderr, "%s [-h] [-m line]\n", programName);
     fprintf(stderr, "\n");
     fprintf(stderr, " -h      : print this help message\n");
     fprintf(stderr, " -m line : number of character per line [default is 80]\n");
}

int main(int argc, char *argv[])
{
    int   c;
    int   maxline                = MAXLINE;
    int   count                  = 0;
    int   bufferUsed             = 0;
    char  buffer[BUFFERSIZE + 1] = { '\0' };


    programName = basename(argv[0]);

    while ( ( c = getopt(argc, argv, "m:") ) != -1 )
    {
        switch ( c )
        {
            case 'm':
                maxline = strtol(optarg, NULL, 0);
                break;

            case '?':
            case 'h':
                printHelp();
                exit(1);
        }
    }

    while ( ( c = getchar() ) != EOF )
    {
        if ( bufferUsed < maxline )
        {
            buffer[bufferUsed++] = c;
            buffer[bufferUsed] = '\0';
        }
        else
        {
            char *cptr = &buffer[bufferUsed - 1];


            while ( ' ' != *cptr
                    && cptr != buffer )
            {
                cptr--;
            }

            // the line input so far has no space, so we print everything out.
            if ( buffer == cptr )
            {
                for ( int n = 0; n < bufferUsed; n++ )
                {
                    putchar(buffer[n]);
                }

                bufferUsed = 0;
                buffer[bufferUsed] = '\0';
            }
            else // the line input has space, so we print the line until space, and then continue to build up line.
            {
                char *iter = buffer;

                while ( iter != cptr )
                {
                    putchar(*iter);
                    iter++;
                }

                  iter = buffer;
                cptr++;
                while ( cptr != &buffer[bufferUsed] )
                {
                    *iter = *cptr;
                    iter++;
                    cptr++;
                }

                *iter = '\0';
                bufferUsed = iter - buffer + sizeof(buffer[0]);

                putchar('\n');
            }

            buffer[bufferUsed++] = c;
            buffer[bufferUsed] = '\0';
        } /* if ( bufferUsed < maxline ) ... else */
    } /* while ( ( c = getchar() ) != EOF ) */

    if ( bufferUsed > 0 )
    {
        int n;

        while ( n < bufferUsed )
        {
            putchar(buffer[n]);
            n++;
        }
    }

    return 0;
}
