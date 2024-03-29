/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * A simple hand-craft scanner (not a token) that will remove C styles of
 * comment, interestingly C style does not allow nested forwardslash asterisk
 * comment, so do this scanner, I shall add nested support sometimes later. :).
 */

#include <stdio.h>

#define YES 1
#define NO 0

/* simple program to remove comments from C
 */
int main(int argc, char *argv[]) {
  int c;
  int isInComment = NO;
  int isCommentSingleLine = NO;
  int isInQuote = NO;
  int isPrevBackSlash = NO;
  int prevC = '\0';

  while ((c = getchar()) != EOF) {
    if (isInComment) {
      if ('/' == c) {
        if ('*' == prevC) {
          isInComment = NO;
          prevC = '\0';
        } else {
          prevC = c;
        }
      } else if ('\n' == c && isCommentSingleLine) {
        isInComment = NO;
        isCommentSingleLine = NO;
        putchar(c);
        prevC = '\0';
      } else {
        prevC = c;
      }
    } else // if ( isInComment )
    {
      if (isInQuote) {
        if ('\\' == c) {
          isPrevBackSlash = YES;
        } else {
          if ('\'' == c) {
            if (!isPrevBackSlash) {
              isInQuote = NO;
            }
          }

          isPrevBackSlash = NO;
        }

        putchar(c);
      } else if ('\'' == c) {
        isInQuote = YES;

        putchar(c);
      } else if ('/' == c) /* ... */
      {
        if ('/' == prevC) //
        {
          isInComment = YES;
          isCommentSingleLine = YES;
        } else {
          prevC = c;
        }
      } else if ('*' == c) {
        if ('/' == prevC) {
          isInComment = YES;
          isCommentSingleLine = NO;
        }

        prevC = '\0';
      } else {
        if ('\0' != prevC) {
          putchar(prevC);
          prevC = '\0';
        }

        putchar(c);
      }
    } // if ( isInComment ) ... else
  }   // while ( ( c = getchar() ) != EOF )
}
