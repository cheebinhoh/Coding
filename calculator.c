/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * A simple calculator that evaluates integer expression with support of 4 binary operators
 * +, -, * and /, and also parethense to override default precedence.
 */


#include <stdio.h>
#include <ctype.h>


char * getNumber(char s[], int *retValue)
{
    char *p     = s;
    int   value = 0;


    while ( isdigit(*p) )
    {
        value = value * 10 + ( *p - '0' );
        p++;
    }    

    *retValue = value;

    return p;
}

char * getOperator(char s[], char *op)
{
    char *p = s;


    if ( '+' == *p 
         || '-' == *p
         || '*' == *p
         || '/' == *p )
    {
        *op = *p;
        p++;
    }

    return p;
}

char * skipWhitespace(char s[])
{
    char *p = s;


    while ( isspace(*p) )
        p++;
       
    return p;
}

int getPrecedence(char op)
{
    int pos = 0;


    switch ( op ) 
    {
        case '+':
        case '-':
            pos = 2;
            break;

        case '*':
            pos = 1;
            break;

        default:
            break;
    }

    return op;
}

int performBinaryOperation(int opr1, int opr2, char op)
{
    int value;


    switch ( op )
    {
        case '*':
            value = opr1 * opr2;
            break;

        case '/':
            value = opr1 / opr2; // let native exception kick in if opr2 is zero
            break;

        case '+':
            value = opr1 + opr2;
            break;

        case '-':
            value = opr1 - opr2;
            break;
    }

    return value;
}

/* it does support negative number, in order to support negative number, we need to:
 * - add a struct to hold operator as token that we can add more semantic meaning than
 *   holding a character ('-' will be the same in prefix negative and binary minus)
 *
 * - the operator token will also maintain position that the token start, so when we 
 *   parse number, we can check if the "-" is immediately before the number
 *
 * - then the rest is evaluating it.
 */ 
int evaluateBinaryExpr(int   number[], 
                       int  *numberIndex,
                       char  op[], 
                       int  *opIndex)
{
    int prevOp;
    int result;
    int opr1;
    int opr2;


    prevOp = op[--(*opIndex)];
    switch ( prevOp )
    {
         case '+':
         case '-':
             opr1 = number[--(*numberIndex)];
             opr2 = number[--(*numberIndex)];
             result = performBinaryOperation(opr1, opr2, prevOp);
             break;

        case '*':
        case '/':
             opr1 = number[--(*numberIndex)];
             opr2 = number[--(*numberIndex)];
             result = performBinaryOperation(opr1, opr2, prevOp);
             break;
    }

    number[(*numberIndex)++] = result;
 
    return result;
}

int evaluate(char s[])
{
    char *tmpP;
    char *p;
    int   numberIndex;
    int   opIndex;
    int   precedenceIndex;
    int   number[100];
    char  op[100];
    int   precedence[100];   
    char  newOp;
    int   newNumber;

    
    p = s;
    numberIndex = 0;
    opIndex = 0;
    precedenceIndex = 0;

    while ( *p != '\0' )
    {
        p = skipWhitespace(p);
    
        tmpP = p;
        if ( '(' == *p )
        {
            precedence[precedenceIndex++] = numberIndex;
            p++;
        }
        else if ( ')' == *p )
        {
            int startNumberIndex = precedence[--precedenceIndex]; 

            if ( opIndex > 0 )
            {
                int loopCnt = numberIndex - startNumberIndex;

                while ( loopCnt >= 2 )
                {
                    evaluateBinaryExpr(number, &numberIndex, op, &opIndex);
                    loopCnt--;
                }   
            }

            p++;
        }
        else if ( ( p = getNumber(p, &newNumber) ) != tmpP )
        {
            number[numberIndex++] = newNumber;
        } 
        else if ( ( p = getOperator(p, &newOp) ) != tmpP )
        {
            if ( opIndex > 0 )
            {
                int prevOp = op[opIndex - 1];
                int prevOpPos = getPrecedence(prevOp);
                int newOpPos = getPrecedence(newOp);
                int opr1;
                int opr2;
                int result;

                if ( prevOpPos <= newOpPos
                     && ( precedenceIndex <= 0
                          || ( numberIndex - precedence[precedenceIndex - 1] ) >= 2 ) )
                {
                    evaluateBinaryExpr(number, &numberIndex, op, &opIndex);
                }       
            }

            op[opIndex++] = newOp;
        }
    }

    while ( opIndex > 0 )
    {
        evaluateBinaryExpr(number, &numberIndex, op, &opIndex);
    }

    return numberIndex <= 0 ? 0 : number[numberIndex - 1];
}


int main(int argc, char *argv[])
{
    char s1[]  = "1 + 2 * 3";
    char s2[] = "( ( 2 + 3 ) * ( 2 + 1 ) ) * 4";
    char s3[]  = "( 2 ) * 3";


    printf("calculation of %s = %d\n", s1, evaluate(s1));
    printf("calculation of %s = %d\n", s2, evaluate(s2));
    printf("calculation of %s = %d\n", s3, evaluate(s3));

    return 0;
}
