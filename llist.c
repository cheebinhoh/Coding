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

    // we can potentially combine the logic inside the if ... then block with logic after it, but
    // it is also more obvious that we check if start matches the value, and do a set of activities,
    // else we search through the list for a matching node and do another set of activities.

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


void freeList(struct ListNode **start)
{
    struct ListNode *tmp;


    while ( NULL != *start )
    {
        tmp    = *start;
        *start = (*start)->next;

        free(tmp);
    }

    *start = NULL;
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


struct ListNode * findNthListNode(struct ListNode *start, int n)
{
     int i;


     if ( NULL == start )
         return NULL;

     i = 0;
     while ( i < n && NULL != start->next )
     {
         start = start->next;
         i++;
     }

     return i == n ? start : NULL;
}


struct ListNode * enQueue(int val, struct ListNode **head)
{
    struct ListNode *node;
    struct ListNode *tmp;


    node = malloc(sizeof( struct ListNode ));
    if ( NULL == node )
        return NULL;

    node->val  = val;
    node->next = NULL;

    if ( NULL == *head )
    {
        *head = node;
    }
    else
    {
        // this can be inefficient if the list is long, a proper way is that the ListNode ADT
        // data also point a pointer to the tail node, so that we can insert without traversal.

        tmp = *head;
        while ( NULL != tmp->next )
            tmp = tmp->next;

        tmp->next = node;
    }

    return node;
}


struct ListNode * deQueue(struct ListNode **head)
{
    struct ListNode *node;


    if ( NULL == *head )
        return NULL;

    node = *head;
    *head = (*head)->next;

    return node;
}


void freeQueue(struct ListNode **head)
{
   struct ListNode *tmp;


   while ( NULL != *head )
   {
       tmp = (*head)->next;
       free(*head);

       *head = tmp;
   }

   *head = NULL;
}


struct ListNode * reverseQueue(struct ListNode *head)
{
    struct ListNode *prev;
    struct ListNode *tmp;


    if ( NULL == head )
        return NULL;

    prev = NULL;
    while ( NULL != head->next )
    {
        tmp        = head->next;
        head->next = prev;
        prev       = head;
        head       = tmp;
    }

    head->next = prev;

    return head;
}


struct ListNode * pushStack(int val, struct ListNode **top)
{
    struct ListNode *node;


    node = malloc(sizeof(struct ListNode));
    if ( NULL == node )
        return NULL;

    node->val  = val;
    node->next = NULL;

    if ( NULL == *top )
        *top = node;
    else
    {
        node->next = *top;
        *top       = node;
    }

    return node;
}


struct ListNode * popStack(struct ListNode **top)
{
    struct ListNode *tmp;


    if ( *top == NULL )
        return NULL;

    tmp  = *top;
    *top = (*top)->next;

    return tmp;
}


void freeStack(struct ListNode **top)
{
    struct ListNode *tmp;


    while ( NULL != *top )
    {
        tmp  = *top;
        *top = (*top)->next;

        free(tmp);
    }

    *top = NULL;
}


int getListLength(struct ListNode *start)
{
    int i = 0;


    while ( NULL != start )
    {
        start = start->next;
        i++;
    }

    return i;
}


void printList(struct ListNode *start)
{
    int len;


    len = getListLength(start);
    if ( len > 0 )
    {
        while ( len > 1
                && NULL != start )
        {
            printf("%d, ", start->val);

            start = start->next;
            len--;
        }

        printf("%d\n", start->val);
    }
}


int isSubsetList(struct ListNode *list, struct ListNode *sublist)
{
    int              llen;
    int              subllen;
    struct ListNode *iter1;
    struct ListNode *iter2;
    struct ListNode *iter3;


    llen    = getListLength(list);
    subllen = getListLength(sublist);
    if ( subllen > llen || llen <= 0 || subllen <= 0 )
        return 0;

    iter1 = list;
    while ( NULL != iter1 )
    {
        iter2 = sublist;
        iter3 = iter1;

        while ( NULL != iter2
                && NULL != iter3
                && iter2->val == iter3->val )
        {
            iter2 = iter2->next;
            iter3 = iter3->next;
        }

        if ( NULL == iter2 ) // is subset
            break;

        iter1 = iter1->next;
    }

    return NULL != iter1;
}


int findListNodeIndex(struct ListNode *start, int val)
{
    int i;


    i = 0;
    while ( NULL != start
            && start->val != val )
    {
        start = start->next;
        i++;
    }

    return NULL == start ? -1 : i;
}
