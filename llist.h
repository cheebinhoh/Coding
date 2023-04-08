/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * A linked list (single) internal.
 */

#ifndef LLIST_H_HAS_INCLUDED

#define LLIST_H_HAS_INCLUDED

#include "llist-internal.h"

// sorted list
struct ListNode *addListNodeInt(int val, struct ListNode **start,
                                struct ListNode **end);
int delListNodeInt(int val, struct ListNode **start, struct ListNode **end);
void freeList(struct ListNode **start);

// queue
struct ListNode *enQueueInt(int val, struct ListNode **head);
struct ListNode *enQueueRef(void *ref, struct ListNode **head);
struct ListNode *deQueue(struct ListNode **head);
struct ListNode *reverseQueue(struct ListNode *head);
void freeQueue(struct ListNode **head);

// stack
struct ListNode *pushStackInt(int val, struct ListNode **top);
struct ListNode *pushStackRef(void *ref, struct ListNode **top);
struct ListNode *popStack(struct ListNode **top);
void freeStack(struct ListNode **top);

// Miscallenous applicable to all
struct ListNode *mergeSortedListInt(struct ListNode *first,
                                    struct ListNode *second);
struct ListNode *findNthListNode(struct ListNode *start, int n);
int getListLength(struct ListNode *start);
int isSubsetList(struct ListNode *list, struct ListNode *sublist);
int findListNodeIntIndex(struct ListNode *start, int val);
int findListNodeRefIndex(struct ListNode *start, void *ref);

void printListInt(struct ListNode *start);
void printListNodeInt(struct ListNode *start);

#endif
