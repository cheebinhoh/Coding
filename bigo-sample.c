#include <stdio.h>

long int recursiveFibonacci(int n) {
  if (n < 2) {
    return n;
  }

  return recursiveFibonacci(n - 1) + recursiveFibonacci(n - 2);
}

int main(int argc, char *argv[]) {
  printf("%ld\n", recursiveFibonacci(6));

  return 0;
}
