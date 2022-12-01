/* Copyright © 2021-2022 Chee Bin HOH. All rights reserved.
 *
 * A simple calculator that evaluates integer expression with support of 4
 * binary operators
 * +, -, *, / and negative unary operator '-', and also parenthese to override
 * default precedence.
 *
 * It does not do syntax and semantic check.
 */

#include <ctype.h>
#include <stdio.h>

struct OpToken {
  char op;
  int opStartPos;
  int opEndPos;
};

char *getNumber(char s[], int *retValue) {
  int value = 0;

  while (isdigit(*s)) {
    value = value * 10 + (*s - '0');
    s++;
  }

  *retValue = value;

  return s;
}

char *getOperator(char s[], char *op) {
  if ('+' == *s || '-' == *s || '*' == *s || '/' == *s) {
    *op = *s;
    s++;
  }

  return s;
}

char *skipWhitespace(char s[]) {
  while (isspace(*s))
    s++;

  return s;
}

int getPrecedence(char op) {
  int pos = 0;

  switch (op) {
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

int performBinaryOperation(int opr1, int opr2, char op) {
  int value;

  switch (op) {
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

int evaluateBinaryExpr(int number[], int *numberIndex, struct OpToken op[],
                       int *opIndex) {
  int prevOp;
  int result;
  int opr1;
  int opr2;

  prevOp = op[--(*opIndex)].op;
  switch (prevOp) {
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

int evaluate(char s[]) {
  char *tmpP;
  char *p;
  int numberIndex;
  int opIndex;
  int precedenceIndex;
  int number[100];
  struct OpToken op[100];
  int precedence[100];
  char newOp;
  int newNumber;

  p = s;
  numberIndex = 0;
  opIndex = 0;
  precedenceIndex = 0;

  while ('\0' != *p) {
    p = skipWhitespace(p);

    tmpP = p;
    if ('(' == *p) {
      precedence[precedenceIndex++] = numberIndex;
      p++;
    } else if (')' == *p) {
      int startNumberIndex = precedence[--precedenceIndex];

      if (opIndex > 0) {
        int loopCnt = numberIndex - startNumberIndex;

        while (loopCnt >= 2) {
          evaluateBinaryExpr(number, &numberIndex, op, &opIndex);
          loopCnt--;
        }
      }

      p++;
    } else if ((p = getNumber(p, &newNumber)) != tmpP) {
      if (opIndex > 0) {
        struct OpToken top;

        top = op[opIndex - 1];
        if ('-' == top.op && top.opEndPos == (tmpP - s)) {
          newNumber = -newNumber;
          opIndex--;
        }
      }

      number[numberIndex++] = newNumber;
    } else if ((p = getOperator(p, &newOp)) != tmpP) {
      if ('-' == newOp && isdigit(*p)) {
        // do nothing as lookahead indicates that the negative sign (-) is
        // prefix to a digit
      } else if (opIndex > 0) {
        int prevOp = op[opIndex - 1].op;
        int prevOpPos = getPrecedence(prevOp);
        int newOpPos = getPrecedence(newOp);
        int opr1;
        int opr2;
        int result;

        if (prevOpPos <= newOpPos &&
            (precedenceIndex <= 0 ||
             (numberIndex - precedence[precedenceIndex - 1]) >= 2)) {
          evaluateBinaryExpr(number, &numberIndex, op, &opIndex);
        }
      }

      op[opIndex].opStartPos = tmpP - s;
      op[opIndex].opEndPos = op[opIndex].opStartPos +
                             1; // all operator are one character long now.
      op[opIndex].op = newOp;
      opIndex++;
    }
  }

  while (opIndex > 0)
    evaluateBinaryExpr(number, &numberIndex, op, &opIndex);

  return numberIndex <= 0 ? 0 : number[numberIndex - 1];
}

int main(int argc, char *argv[]) {
  char s1[] = "1 + 2 * 3";
  char s2[] = "( ( 2 + 3 ) * ( 2 + 1 ) ) * 4";
  char s3[] = "( 2 ) * 3";
  char s4[] = "3 + -2";
  char s5[] = "( 3 + ( -1 ) ) * 3";

  printf("calculation of %s = %d\n", s1, evaluate(s1));
  printf("calculation of %s = %d\n", s2, evaluate(s2));
  printf("calculation of %s = %d\n", s3, evaluate(s3));
  printf("calculation of %s = %d\n", s4, evaluate(s4));
  printf("calculation of %s = %d\n", s5, evaluate(s5));

  return 0;
}
