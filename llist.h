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


// queue
struct ListNode * enqueue(int val, struct ListNode **head);
struct ListNode * dequeue(struct ListNode **head);

// stack
struct ListNode * pushStack(int val, struct ListNode **top);
struct ListNode * popStack(struct ListNode **top);
void              delStack(struct ListNode *top);

#endif
