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


void freeStack(struct ListNode *top)
{
    struct ListNode *tmp;


    while ( NULL != top )
    {
        tmp = top;
        top = top->next;

        free(tmp);
    }
}
