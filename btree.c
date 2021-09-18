/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * ADT binary search tree
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "btree.h"
#include "llist.h"


void findTreeNodeAndParentRecursive(struct TreeNode   *root,
                                    int                val,
                                    struct TreeNode  **retNode,
                                    struct TreeNode ***retParent)
{
    if ( NULL == root )
    {
        if ( NULL != retParent )
            *retParent = NULL;

        *retNode = NULL;

        return;
    }

    if ( val == root->val )
    {
        *retNode = root;
    }
    else if ( val < root->val
              && NULL != root->left )
    {
        if (NULL != retParent)
            *retParent = &(root->left);

        findTreeNodeAndParentRecursive(root->left, val, retNode, retParent);
    }
    else if ( NULL != root->right )
    {
        if (NULL != retParent)
            *retParent = &(root->right);

        findTreeNodeAndParentRecursive(root->right, val, retNode, retParent);
    }
}


/* WARNING that caller is responsible to set the dereferenced object if parent
 * is not NULL as upon return if the deleted node is root, then the recursive
 * function will not the value into location at parent as there is no way
 * we can tell what is the address of the root in caller of this method via
 * to levels of method calls.
 */
void findTreeNodeAndParent(struct TreeNode   *root,
                           int                val,
                           struct TreeNode  **node,
                           struct TreeNode ***parent)
{
    findTreeNodeAndParentRecursive(root, val, node, parent);
}


struct TreeNode * findTreeNode(struct TreeNode *root, int val)
{
    struct TreeNode *ret = NULL;


    findTreeNodeAndParent(root, val, &ret, NULL);

    return ret;
}


struct TreeNode * addTreeNode(struct TreeNode *root, int val)
{
    struct TreeNode *node;


    if ( NULL == root )
    {
        node = malloc(sizeof( struct TreeNode )); // I do not care about NULL error as it is just a test
        assert( NULL != node );

        node->val   = val;
        node->left  = NULL;
        node->right = NULL;
    }
    else
    {
        node = root;


        if ( val == node->val )
        {
            // do nothing
        }
        else if ( val < node->val )
        {
            node->left = addTreeNode(node->left, val);
        }
        else
        {
            node->right = addTreeNode(node->right, val);
        }
    }

    return node;
}


struct TreeNode * delTreeNode(struct TreeNode *root, int val)
{
    struct TreeNode **parent;
    struct TreeNode  *node;
    int               leftLevel;
    int               rightLevel;


    if ( NULL == root )
        return NULL;

    node   = NULL;
    parent = &root;
    findTreeNodeAndParent(root, val, &node, &parent);
    if ( NULL != node )
    {
        leftLevel  = determineMaxDepthLevel(node->left);
        rightLevel = determineMaxDepthLevel(node->right);

        if ( leftLevel >= rightLevel )
        {
            struct TreeNode *farRight;
            struct TreeNode *farLeft;
            struct TreeNode *prev;


            prev     = NULL;
            farRight = node->left;
            while ( NULL != farRight
                    && NULL != farRight->right )
            {
                prev     = farRight;
                farRight = farRight->right;
            }

            if ( NULL != farRight )
            {
                farRight->right = node->right;
                if ( farRight != node->left )
                {
                    prev->right = NULL;
                    farLeft     = farRight;

                    while ( NULL != farLeft
                            && NULL != farLeft->left )
                       farLeft = farLeft->left;

                    farLeft->left = node->left;
                }
            }

            *parent  = farRight;
        }
        else
        {
            struct TreeNode *farRight;
            struct TreeNode *farLeft;
            struct TreeNode *prev;


            prev     = NULL;
            farLeft = node->right;
            while ( NULL != farLeft
                    && NULL != farLeft->left )
            {
                prev    = farLeft;
                farLeft = farLeft->left;
            }

            if ( NULL != farLeft )
            {
                farLeft->left = node->left;
                if ( farLeft!= node->right )
                {
                    prev->left = NULL;
                    farRight    = farLeft;

                    while ( NULL != farRight
                            && NULL != farRight->right )
                        farRight = farRight->right;

                    farRight->right = node->right;
                }
            }

            *parent  = farLeft;
        }

        free(node);
    }

    return root;
}


int determineMaxDepthLevelRecursive(struct TreeNode *root,
                                    int              level)
{
    int rightLevel;
    int leftLevel;


    if ( NULL == root )
        return level;

    rightLevel = determineMaxDepthLevelRecursive(root->left, level + 1);
    leftLevel = determineMaxDepthLevelRecursive(root->right, level + 1);

    return ( rightLevel > leftLevel ) ? rightLevel : leftLevel;
}


int determineMaxDepthLevel(struct TreeNode *root)
{
    return determineMaxDepthLevelRecursive(root, 0);
}


int determineMinDepthLevelRecursive(struct TreeNode *root,
                                    int              level,
                                    int             *retMinLevel)
{
    int leftLevel;
    int rightLevel;
    int oldMinLevel;


    if ( NULL == root )
    {
        if ( level < *retMinLevel )
            *retMinLevel = level;

        return level;
    }

    level       = level + 1;
    oldMinLevel = *retMinLevel;
    leftLevel   = determineMinDepthLevelRecursive(root->left, level, retMinLevel);

    if ( leftLevel == level
         && NULL != root->right )
        *retMinLevel = oldMinLevel;

    oldMinLevel = *retMinLevel;
    rightLevel  = determineMinDepthLevelRecursive(root->right, level, retMinLevel);

    if ( rightLevel == level
         && NULL != root->left )
        *retMinLevel = oldMinLevel;

    return ( leftLevel < rightLevel ) ? leftLevel : rightLevel; 
}


int determineMinDepthLevel(struct TreeNode *root)
{
    int minLevel;
    int i;


    i        = 1;
    minLevel = i;
    while ( i > 0 )
    {
        minLevel = i;
        i = i << 1;
        i = i + 1;
    }

    determineMinDepthLevelRecursive(root, 0, &minLevel);

    return minLevel;
}


void printTreeNodeInTreeTopologyRecursive(struct TreeNode *root,
                                          int              level)
{
    int i;


    for ( i = 0; i < level * 5; i++ )
        putchar(' ');

    if ( NULL == root)
    {
        printf("-\n");
        return;
    }

    printf("%d (L=%d, R=%d)\n",
           root->val,
           determineMaxDepthLevel(root->left),
           determineMaxDepthLevel(root->right));

    printTreeNodeInTreeTopologyRecursive(root->right, level + 1);
    printTreeNodeInTreeTopologyRecursive(root->left, level + 1);
}


void printTreeNodeInTreeTopology(struct TreeNode *root)
{
    printTreeNodeInTreeTopologyRecursive(root, 0);
}


void traverseTreeNodeInOrderRecursive(struct TreeNode        *root,
                                      int                    *pos,
                                      bTreeTraversalCallback  func,
                                      void                   *data)
{
    if ( NULL == root )
        return;

    traverseTreeNodeInOrderRecursive(root->left, pos, func, data);

    func(root, *pos, data);
    *pos = *pos + 1;

    traverseTreeNodeInOrderRecursive(root->right, pos, func, data);
}


void traverseTreeNodeInOrder(struct TreeNode *root, bTreeTraversalCallback func, void *data)
{
    int pos;


    pos = 0;
    traverseTreeNodeInOrderRecursive(root, &pos, func, data);
}


void countTreeNode(struct TreeNode *node, int pos, void *data)
{
    int *count;


    count  = data;
    *count = *count + 1;
}


int findTotalNumberOfTreeNode(struct TreeNode *root)
{
    int count = 0;


    traverseTreeNodeInOrder(root, countTreeNode, &count);

    return count;
}


void freeTreeNode(struct TreeNode *root)
{
    if ( NULL == root )
        return;

    if ( NULL != root->left )
        freeTreeNode(root->left);

    if ( NULL != root->right )
        freeTreeNode(root->right);

    free(root);
}


int isSmallerThanList(int val, struct ListNode *list)
{
    if ( NULL == list )
       return 1;

    do {
        if ( val > list->val )
            return 0;

        list = list->next;
    } while ( NULL != list );

    return 1;
}


int isLargerThanList(int val, struct ListNode *list)
{
    if ( NULL == list )
       return 1;

    do {
        if ( val < list->val )
            return 0;

        list = list->next;
    } while ( NULL != list );

    return 1;
}


/* In this approach, we maintain a two lists:
 * - smaller is a list of values that visit node value must be smaller than any value in the list
 * - larger is a list of values that visit node value must be larger than any value in the list
 *
 * The other approach is to traverse the whole tree in in order list a
 * and passing last visit value from node to node and where new node value should not exceed last
 * visit node if it is a binary search tree.
 */
int isTreeBinarySearchTreeRecursive(struct TreeNode *root,
                                    struct ListNode *smaller,
                                    struct ListNode *larger)
{
    struct ListNode *tmp;


    if ( NULL == root )
        return 1;

    if ( ! isSmallerThanList(root->val, smaller) )
        return 0;

    if ( ! isLargerThanList(root->val, larger) )
        return 0;

    pushStack(root->val, &smaller);
    if ( ! isTreeBinarySearchTreeRecursive(root->left, smaller, larger) )
        return 0;

    tmp = popStack(&smaller);
    free(tmp);

    pushStack(root->val, &larger);
    if ( ! isTreeBinarySearchTreeRecursive(root->right, smaller, larger) )
        return 0;

    tmp = popStack(&larger);
    free(tmp);

    return 1;
}


int isTreeBinarySearchTree(struct TreeNode *root)
{
    struct ListNode *smaller = NULL;
    struct ListNode *larger  = NULL;
    int              ret;


    ret = isTreeBinarySearchTreeRecursive(root, smaller, larger);

    freeStack(&smaller);
    freeStack(&larger);

    return ret;
}


struct TreeNode * findLeastCommonAncestorInternal(struct TreeNode *root,
                                                  int              val1,
                                                  int              val2)
{
    struct TreeNode *node;


    if ( NULL == root )
        return NULL;

    if ( val1 == val2
         && val2 == root->val )
        return root;

    if ( NULL == root->left
         && NULL == root->right )
        return NULL;

    if ( NULL == root->left )
    {
        if ( root->val == val1
             && findTreeNode(root->right, val2) != NULL )
            return root;

        if ( root->val == val2
             && findTreeNode(root->right, val1) != NULL )
            return root;

        return findLeastCommonAncestorInternal(root->right, val1, val2);
    }

    if ( NULL == root->right )
    {
        if ( root->val == val1
             && findTreeNode(root->left, val2) != NULL )
            return root;

        if ( root->val == val2
             && findTreeNode(root->left, val1) != NULL )
            return root;

        return findLeastCommonAncestorInternal(root->left, val1, val2);
    }

    if ( findTreeNode(root->left, val1) != NULL
         && findTreeNode(root->right, val2) != NULL )
        return root;

    if ( findTreeNode(root->left, val2) != NULL
         && findTreeNode(root->right, val1) != NULL )
        return root;

    node = findLeastCommonAncestorInternal(root->left, val1, val2);
    if ( NULL != node )
        return node;

    node = findLeastCommonAncestorInternal(root->right, val1, val2);
    if ( NULL != node )
        return node;

    return NULL;
}


struct TreeNode * findLeastCommonAncestor(struct TreeNode *root,
                                          int              val1,
                                          int              val2)
{
    return findLeastCommonAncestorInternal(root, val1, val2);
}


int findMaximumSumPathToLeafValueRecursive(struct TreeNode *root, int val, int *sum)
{
    int ret;
    int lret;
    int lsum;
    int rret;
    int rsum;


    ret = 0;

    if ( NULL == root )
        return ret;

    *sum = *sum + root->val;

    if ( NULL == root->left
         && NULL == root->right )
    {
        ret = root->val == val;
    }
    else
    {
       lsum = *sum;
       lret = 0;
       rsum = *sum;
       rret = 0;


       if ( NULL != root->left )
           lret = findMaximumSumPathToLeafValueRecursive(root->left, val, &lsum);

       if ( lret )
           *sum = lsum;

       if ( NULL != root->right )
       {
           rret = findMaximumSumPathToLeafValueRecursive(root->right, val, &rsum);

           if ( rret && rsum > *sum )
               *sum = rsum;
       }

       ret = lret || rret;
    }

    if ( ! ret )
        *sum = *sum - root->val;  // backtracking :)

    return ret;
}


int findMaximumSumPathToLeafValue(struct TreeNode *root, int val)
{
    int sum = 0;


    findMaximumSumPathToLeafValueRecursive(root, val, &sum);

    return sum;
}


/* Have a small function that does one thing at a time, a similar philosophy from
 * Unix that has program that does one thing at a time and does it well. :)
 *
 * So we do not have a monolithic function that execute behavior of pre, in and post order.
 */
void getInOrderListRecursively(struct TreeNode *root, struct ListNode **start)
{
    if ( NULL == root )
        return;

    getInOrderListRecursively(root->left, start);

    enQueue(root->val, start);

    getInOrderListRecursively(root->right, start);
}


struct ListNode * getInOrderList(struct TreeNode *root)
{
    struct ListNode *start = NULL;


    getInOrderListRecursively(root, &start);

    return start;
}


void getPreOrderListRecursively(struct TreeNode *root, struct ListNode **start)
{
    if ( NULL == root )
        return;

    enQueue(root->val, start);

    getPreOrderListRecursively(root->left, start);
    getPreOrderListRecursively(root->right, start);
}


struct ListNode * getPreOrderList(struct TreeNode *root)
{
    struct ListNode *start = NULL;


    getPreOrderListRecursively(root, &start);

    return start;
}


void getPostOrderListRecursively(struct TreeNode *root, struct ListNode **start)
{
    if ( NULL == root )
        return;

    getPostOrderListRecursively(root->left, start);
    getPostOrderListRecursively(root->right, start);

    enQueue(root->val, start);
}


struct ListNode * getPostOrderList(struct TreeNode *root)
{
    struct ListNode *start = NULL;


    getPostOrderListRecursively(root, &start);

    return start;
}


int isSubBinaryTree(struct TreeNode *tree1, struct TreeNode *tree2)
{
    struct ListNode *list1;
    struct ListNode *list2;
    int              ret;


    ret = 0;

    list1 = getPostOrderList(tree1);
    list2 = getPostOrderList(tree2);
    if ( ! isSubsetList(list1, list2) )
        goto cleanup;

    freeList(&list1);
    freeList(&list2);
 
    list1 = getInOrderList(tree1);
    list2 = getInOrderList(tree2);
    if ( ! isSubsetList(list1, list2 ) )
        goto cleanup;

    ret = 1;

cleanup:
    freeList(&list1);
    freeList(&list2);

    return ret;
}
