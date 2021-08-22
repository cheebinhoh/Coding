/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Some common C string utlities.
 */

#include <stdio.h>
#include <ctype.h>


int atoi(char s[]);
void toupperString(char s[]);
void squeeze1(char s[], char c);
void squeeze2(char s[], char c);
int strindex(char s[], char t[]);
double atof(char s[]);
void reverse(char s[]);


int main(int argc, char *argv[])
{
    char s[] = "C++ is elegant and beautiful";
    char c   = '+';


    printf("Original string is \"%s\"\n", s);
    squeeze2(s, c);
    printf("Squenze + out, result is \"%s\"\n", s);

    return 0;
}


/* convert a string into a double
 */
double atof(char s[])
{
    double value         = 0.0;
    int    power         = 1;
    int    fractionValue = 0;
    int    sign          = 1;
    int    i;


    for ( i = 0; isspace(s[i]); i++ )
        ;

    if ( '-' == s[i] )
    {
        sign = -1;
        i++;
    }

    while ( isdigit(s[i]) )
    {
        value = value * 10 + ( s[i] - '0' );
        i++;
    }

    if ( '.' == s[i++] )
    {
        while ( isdigit(s[i]) )
        {
            power         = power * 10;
            fractionValue = fractionValue * 10 + ( s[i] - '0' );

            i++;
        }

        value = value + ( fractionValue * ( 1.0 / power ) );

        if ( 'e' == s[i++] )
        {
            int exponentSign  = 1;
            int exponentValue = 0;


            if ( '-' == s[i] )
            {
                exponentSign = -1;

                i++;
            }
            else if ( '+' == s[i] )
            {
                i++;
            }

            while ( isdigit(s[i]) )
            {
                exponentValue = exponentValue * 10 + ( s[i] - '0' );

                i++;
            }

            power = 1;
            while ( exponentValue > 0 )
            {
                power = power * 10;

                exponentValue--;
            }

            value = value * ( exponentSign < 0 ? 1.0 / power : power );
        }
    }

    return value;
}


/* the logic is simple:
 * - i is moved along the 1st array
 * - j is only moved when i and j elements are matched
 * - whenever i and j elements are not matched, we reset j to 0
 * - at the end of the function, we check if j is end of matched string, if so, we return
 *   i - j position (1st array), else -1.
 */
int strindex(char s[], char t[])
{
    int i;
    int j;


    for ( i = 0, j = 0; s[i] != '\0' && t[j] != '\0'; i++ )
    {
        if ( s[i] == t[j] )
        {
            j++;
        }
        else
        {
            j = 0;
        }
    }

    return t[j] == '\0' ? i - j : -1;
}


int atoi(char s[])
{
    int value = 0;
    int i;


    for ( i = 0; s[i] != '\0'; i++ )
        value = ( value * 10 ) + ( s[i] - '0' );

    return value;
}

void toupperString(char s[])
{
    int i = 0;


    while ( s[i] != '\0' )
    {
        if ( s[i] >= 'a' && s[i] <= 'z' )
        {
            s[i] = 'A' + ( s[i] - 'a' );
        }

        i++;
    }
}


/* This is a left to right version that we rip off a character from
 * a string.
 *
 * This is an inefficient version as whenever we find a character to
 * be removed, we move all characters to its right one position up,
 * those characters might include one that we do not want.
 */
void squeeze1(char s[], char c)
{
    int i;


    for ( i = 0; '\0' != s[i]; i++ )
    {
        if ( c == s[i] )
        {
            int j = i;


            while ( ( s[j] = s[j + 1] ) != '\0' )
                j++;

            i--; // repeat this new position check
        }
    }
}

/* This ia an efficient version that we remove character
 * from the right, so when we move character one position to
 * the left, we know that characters on the right has no
 * characters that we do not want.
 */
void squeeze2(char s[], char c)
{
    int i;
    int j;


    // find the end
    i = 0;
    while ( '\0' != s[i] )
        i++;

    while ( i > 0 )
    {
        i--;

        if ( c == s[i] )
        {
            j = i;


            while ( ( s[j] = s[j + 1] ) != '\0' )
                j++;
        }
    }
}

void reverse(char s[])
{
    char *lastp;
    char *p;
    char  tmpc;


    lastp = s;
    p     = s;
    while ( '\0' != *p )
    {
        lastp = p;
        p++;
    }

    p = s;
    while ( p != lastp )
    {
        tmpc   = *p;
        *p     = *lastp;
        *lastp = tmpc;

        p++;
        lastp--;
    }
}
