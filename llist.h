/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * A linked list (single) internal
 */

#ifndef LLIST_H_HAS_INCLUDED

#define LLIST_H_HAS_INCLUDED

#include "llist-internal.h"

struct ListNode * addListNode(int val, struct ListNode **start, struct ListNode **end);
int               delListNode(int val, struct ListNode **start, struct ListNode **end);
void              freeListNode(struct ListNode *start);

void printListNode(struct ListNode *start);

#endif
