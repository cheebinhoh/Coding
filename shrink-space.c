/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * It shrinks consecutive spaces into one space.
 */

#include <stdio.h>


int main(int argc, char *argv[])
{
    int c;
    int prevIsBlank = 0;


    while ( ( c = getchar() ) != EOF )
    {
        if ( ' ' == c )
        {
            if ( ! prevIsBlank )
            {
                putchar(c);
                prevIsBlank = 1;
            }
        }
        else
        {
            prevIsBlank = 0;

            putchar(c);
        }
    }

    return 0;
}
