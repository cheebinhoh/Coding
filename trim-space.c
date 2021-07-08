/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * It trims trailing space or tab at the end of line or file.
 */

#include <stdio.h>
#include <ctype.h>


struct Token
{
   char  ch;
   long  count;
};


int main(int argc, char *argv[])
{
    int c;
    int i;
    int j;
    int tokenIndex = 0;
    int numOfNewLine = 0;
    struct Token tokenList[100];


    while ( ( c = getchar() ) != EOF )
    {
       if ( isspace(c) )
       {
           if ( '\n' == c )
           {
               if ( numOfNewLine <= 0 )
               {
                   putchar(c);
               }

               tokenIndex = 0;
               numOfNewLine++;
           }
           else
           {
               if ( tokenIndex <= 0
                    || tokenList[tokenIndex - 1].ch != c )
               {
                   tokenList[tokenIndex].ch = c;
                   tokenList[tokenIndex].count = 1;
                   tokenIndex++;
               }
               else
               {
                   tokenList[tokenIndex - 1].count++;
               }
           }
       }
       else
       {
           while ( numOfNewLine > 1 )
           {
               putchar('\n');
               numOfNewLine--;
           }

           for ( i = 0; i < tokenIndex; i++ )
           {
               for ( j = 0; j < tokenList[i].count; j++ )
               {
                   putchar(tokenList[i].ch);
               }
           }

           tokenIndex = 0;
           numOfNewLine = 0;
           putchar(c);
       }
    }

    return 0;
}
