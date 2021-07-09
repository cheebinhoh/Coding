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

int space2Tab = 8;

int main(int argc, char *argv[])
{
    int c;
    int startWithNoneSpace = 0;

    while ( ( c = getchar() ) != EOF )
    {
        if ( '\t' == c 
             && !startWithNoneSpace )
        {
            int i;

            for ( i = 0; i < space2Tab; i++ )
            {        
                putchar(' ');
            }
        }
        else
        {
            if (! isspace(c ) )
            {
               startWithNoneSpace = 1;
            }
            else if ( '\n' == c )
            {
               startWithNoneSpace = 0;
            }

            putchar(c);
        }
    }

    return 0;
}
