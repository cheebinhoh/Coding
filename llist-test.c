/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Test linked list (single)
 */

#include "llist.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  struct ListNode *start = NULL;
  struct ListNode *end = NULL;

  addListNodeInt(3, &start, &end);
  addListNodeInt(2, &start, &end);
  addListNodeInt(4, &start, &end);
  addListNodeInt(1, &start, &end);

  printf("The list of integer is: ");
  printListNodeInt(start);
  printf("\n");

  printf("After deleting 2\n");
  delListNodeInt(2, &start, &end);
  printListNodeInt(start);
  printf("\n");

  return 0;
}
