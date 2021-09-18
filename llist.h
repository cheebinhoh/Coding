/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * A linked list (single) internal
 */

#ifndef LLIST_H_HAS_INCLUDED

#define LLIST_H_HAS_INCLUDED

#include "llist-internal.h"


// sorted list
struct ListNode * addListNode(int val, struct ListNode **start, struct ListNode **end);
int               delListNode(int val, struct ListNode **start, struct ListNode **end);
void              freeList(struct ListNode *start);
struct ListNode * findNthListNode(struct ListNode *start, int n);

void printListNode(struct ListNode *start);


// queue
struct ListNode * enQueue(int val, struct ListNode **head);
struct ListNode * deQueue(struct ListNode **head);
struct ListNode * reverseQueue(struct ListNode *head);
void              freeQueue(struct ListNode **head);

// stack
struct ListNode * pushStack(int val, struct ListNode **top);
struct ListNode * popStack(struct ListNode **top);
void              freeStack(struct ListNode *top);

// Miscallenous applicable to all
int getListLength(struct ListNode *start);
void printList(struct ListNode *start);
int isSubsetList(struct ListNode *list, struct ListNode *sublist);


#endif
