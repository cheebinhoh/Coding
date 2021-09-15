/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * A linked list (single) internal
 */

#include <stdio.h>
#include <stdlib.h>
#include "llist.h"


int delListNode(int val, struct ListNode **start, struct ListNode **end)
{
    struct ListNode *tmp;
    struct ListNode *del;


    if ( NULL == (*start) )
        return 0;

    if ( (*start)->val == val )
    {
        del    = (*start);
        *start = del->next;
        if ( NULL != end
             && del == *end )
            *end = *start;

        free(del);

        return 1;
    }

    tmp = *start;
    while ( NULL != tmp->next
            && val != tmp->next->val )
        tmp = tmp->next;

    if ( NULL != tmp->next )
    {
        del       = tmp->next;
        tmp->next = del->next;
        if ( NULL != end
             && del == *end )
            *end = tmp;

        free(del);

        return 1;
    }

    return 0;
}

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
         || val < (*start)->val )
    {
        node->next = *start;
        *start     = node;
    }
    else if ( val > (*start)->val )
    {
        tmp = *start;
        while ( NULL != tmp->next
                && val > tmp->next->val )
            tmp = tmp->next;

        node->next = tmp->next;
        tmp->next  = node;
    }
    else
    {
        free(node);
        node = NULL;
    }

    if ( NULL != node
         && NULL == node->next )
        if ( NULL != end )
            *end = *start;

    return node;
}

void freeListNode(struct ListNode *start)
{
    struct ListNode *tmp;


    while ( NULL != start )
    {
        tmp   = start;
        start = start->next;

        free(tmp);
    }
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
