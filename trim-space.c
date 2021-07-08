/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * It trims trailing space or tab 
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
    struct Token tokenList[100];
    

    while ( ( c = getchar() ) != EOF )
    {
       if ( isspace(c) )
       {
           if ( '\n' == c )
           {
               tokenIndex = 0;
               putchar(c);
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
           for ( i = 0; i < tokenIndex; i++ )
           {
               for ( j = 0; j < tokenList[i].count; j++ )
               {
                   putchar(tokenList[i].ch);
               }
           }

           tokenIndex = 0;
           putchar(c);
       }
    }

    return 0;
}

