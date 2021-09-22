/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Find the vertical sum of a binary tree. Assume that the left and right child
 * of a node makes 45 degree angle with the parent.
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"
#include "llist.h"


void calculateBTreeVerticalSumRecursive(struct TreeNode  *root,
                                        int               vAxis,
                                        struct ListNode **left,
                                        struct ListNode **right)
{
    struct ListNode *node;


    if ( NULL == root )
        return;

    if ( vAxis >= 0 )
    {
        node = findNthListNode(*right, vAxis);
        if ( NULL == node )
            enQueueInt(root->val, right);
        else
            node->data.val += root->val;
    }
    else
    {
        int adjustedVAxis = ( vAxis * -1 ) - 1;

        node = findNthListNode(*left, adjustedVAxis);
        if ( NULL == node )
            enQueueInt(root->val, left);
        else
            node->data.val += root->val;
    }

    calculateBTreeVerticalSumRecursive(root->left, vAxis - 1, left, right);
    calculateBTreeVerticalSumRecursive(root->right, vAxis + 1, left, right);
}

struct ListNode * calculateBTreeVerticalSum(struct TreeNode *root)
{
    struct ListNode *rightlist = NULL;
    struct ListNode *leftlist  = NULL;
    struct ListNode *queueIter = NULL;


    if ( NULL == root )
        return NULL;

    calculateBTreeVerticalSumRecursive(root, 0, &leftlist, &rightlist);

    leftlist = reverseQueue(leftlist);
    queueIter = rightlist;
    while ( NULL != queueIter )
    {
        enQueueInt(queueIter->data.val, &leftlist);
        queueIter = queueIter->next;
    }

    freeQueue(&rightlist);

    return leftlist;
}

/*
 */
int main(int argc, char * argv[])
{
    struct TreeNode *root;
    struct TreeNode *other;
    struct ListNode *resultlist;
    struct ListNode *listIter;


    root = NULL;
    root = malloc(sizeof( struct TreeNode ) );
    root->val   = 1;
    root->left  = NULL;
    root->right = NULL;

    other = malloc(sizeof( struct TreeNode ) );
    other->val   = 2;
    other->left  = NULL;
    other->right = NULL;
    root->left   = other;

    other = malloc(sizeof( struct TreeNode ) );
    other->val   = 3;
    other->left  = NULL;
    other->right = NULL;
    root->right  = other;

    other              = malloc(sizeof( struct TreeNode ) );
    other->val         = 6;
    other->left        = NULL;
    other->right       = NULL;
    root->right->right = other;

    other             = malloc(sizeof( struct TreeNode ) );
    other->val        = 5;
    other->left       = NULL;
    other->right      = NULL;
    root->right->left = other;

    other                   = malloc(sizeof( struct TreeNode ) );
    other->val              = 7;
    other->left             = NULL;
    other->right            = NULL;
    root->right->left->left = other;

    other                    = malloc(sizeof( struct TreeNode ) );
    other->val               = 8;
    other->left              = NULL;
    other->right             = NULL;
    root->right->left->right = other;

    printf("The tree topology:\n");
    printf("\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");

    printf("\n");
    printf("The tree in aligned vertical axis\n");
    printf("\n");
    printf("\t\t1\n");
    printf("\n");
    printf("\t2\t\t3\n");
    printf("\n");
    printf("\t\t5\t\t6\n");
    printf("\n");
    printf("\t7\t\t8\n");
    printf("\n");

    printf("\n");
    printf("Vertical node sum:\n");
    printf("\n");
    resultlist = calculateBTreeVerticalSum(root);
    listIter = resultlist;
    while ( NULL != listIter )
    {
        printf("\t%d", listIter->data.val);
        listIter = listIter->next;
    }

    printf("\n");
    printf("\n");

    freeQueue(&resultlist);

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    return 0;
 }
