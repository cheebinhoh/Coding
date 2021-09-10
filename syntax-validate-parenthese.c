/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Validate that a C or C like program has right number of open and close parathenses.
 */

#include <stdio.h>

#define YES 1
#define NO  0


int main(int argc, char *argv[])
{
    int c;
    int numOfOpenParenthese = 0;
    int inString            = NO;


    while ( ( c = getchar() ) != EOF )
    {
        if ( '"' == c )
        {
            inString = !inString;
        }
        else if ( ! inString )
        {
            if ( '(' == c )
            {
                numOfOpenParenthese++;
            }
            else if ( ')' == c )
            {
                numOfOpenParenthese--;
            }
        }
    }

    if ( 0 == numOfOpenParenthese )
        printf("has valid parenthese\n");
    else if ( numOfOpenParenthese > 0 )
        printf("missing close parenthese\n");
    else
        printf("missing open parenthese\n");

    return 0;
}
