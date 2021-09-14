/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * A linked list (single) internal
 */

#include <stdio.h>
#include <stdlib.h>
#include "llist.h"

struct ListNode * addListNode(int val, struct ListNode **start, struct ListNode **end)
{
    struct ListNode *node;
    struct ListNode *tmp;


    node = malloc(sizeof(struct ListNode));
    if ( NULL == node )
        return node;

    node->val  = val;
    node->next = NULL;

    if ( NULL == *start
         || val <= (*start)->val )
    {
        node->next = *start;
        *start     = node;
    }
    else
    {
        tmp = *start;
        while ( NULL != tmp->next
                && val > tmp->next->val )
            tmp = tmp->next;

        node->next = tmp->next;
        tmp->next  = node;
    }

    if ( NULL == node->next )
        if ( NULL != end )
            *end = *start;

    return node;
}

void printListNode(struct ListNode *start)
{
    while ( NULL != start )
    {
        printf("%d -> ", start->val);
        start = start->next;
    }

    printf("-\n");
}
