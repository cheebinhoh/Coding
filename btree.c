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


int traverseTreeNodePreOrderRecursive(struct TreeNode        *root,
                                      int                    *pos,
                                      bTreeTraversalCallback  func,
                                      void                   *data)
{
    int stop = 0;


    if ( NULL == root )
        return 0;

    func(root, *pos, &stop, data);
    if ( stop )
        return stop;

    *pos = *pos + 1;

    stop = traverseTreeNodePreOrderRecursive(root->left, pos, func, data);
    if ( stop )
        return stop;

    return traverseTreeNodePreOrderRecursive(root->right, pos, func, data);
}


void traverseTreeNodePreOrder(struct TreeNode *root, bTreeTraversalCallback func, void *data)
{
    int pos;


    pos = 0;
    traverseTreeNodePreOrderRecursive(root, &pos, func, data);
}


int traverseTreeNodeInOrderRecursive(struct TreeNode        *root,
                                     int                    *pos,
                                     bTreeTraversalCallback  func,
                                     void                   *data)
{
    int stop = 0;


    if ( NULL == root )
        return 0;

    stop = traverseTreeNodeInOrderRecursive(root->left, pos, func, data);
    if ( stop )
        return stop;

    func(root, *pos, &stop, data);
    if ( stop )
        return stop;

    *pos = *pos + 1;

    return traverseTreeNodeInOrderRecursive(root->right, pos, func, data);
}


void traverseTreeNodeInOrder(struct TreeNode *root, bTreeTraversalCallback func, void *data)
{
    int pos;


    pos = 0;
    traverseTreeNodeInOrderRecursive(root, &pos, func, data);
}


int traverseTreeNodePostOrderRecursive(struct TreeNode        *root,
                                       int                    *pos,
                                       bTreeTraversalCallback  func,
                                       void                   *data)
{
    int stop = 0;


    if ( NULL == root )
        return 0;

    stop = traverseTreeNodePostOrderRecursive(root->left, pos, func, data);
    if ( stop )
        return stop;

    stop = traverseTreeNodePostOrderRecursive(root->right, pos, func, data);
    if ( stop )
        return stop;

    func(root, *pos, &stop, data);
    *pos = *pos + 1;

    return stop;
}


void traverseTreeNodePostOrder(struct TreeNode *root, bTreeTraversalCallback func, void *data)
{
    int pos;


    pos = 0;
    traverseTreeNodePostOrderRecursive(root, &pos, func, data);
}


void countTreeNode(struct TreeNode *node, int pos, int *stop, void *data)
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
        if ( val > list->data.val )
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
        if ( val < list->data.val )
            return 0;

        list = list->next;
    } while ( NULL != list );

    return 1;
}


static void getTreeSubbranchAsList(struct TreeNode  *root,
                                   int               left,
                                   int               missingVal,
                                   struct ListNode **head)
{
    if ( NULL == root )
    {
        enQueueInt(missingVal, head);

        return;
    }

    enQueueInt(root->val, head);
    if ( left )
    {
        getTreeSubbranchAsList(root->left, left, missingVal, head);
        getTreeSubbranchAsList(root->right, left, missingVal, head);
    }
    else
    {
        getTreeSubbranchAsList(root->right, left, missingVal, head);
        getTreeSubbranchAsList(root->left, left, missingVal, head);
    }
}

int isTreeSymmetric(struct TreeNode *root)
{
    int              i;
    int              missingVal;
    struct ListNode *leftList;
    struct ListNode *rightList;
    struct ListNode *leftIter;
    struct ListNode *rightIter;


    if ( NULL == root )
        return 1;


    i          = 1;
    missingVal = i;
    while ( i > 0 )
    {
        missingVal = i;
        i = i << 1;
        i = i + 1;
    }

    // C programming language is simple, beautiful and concise and high performance, but it
    // does come with certain deficiency in term of moderm programming language, like it does
    // insist initialization of variable, programmer is either discipline or runtime stack
    // decide what value you got :)

    leftList  = NULL;
    rightList = NULL;
    getTreeSubbranchAsList(root->left, 1, missingVal, &leftList);
    getTreeSubbranchAsList(root->right, 0, missingVal, &rightList);

    i = getListLength(leftList);
    if ( i != getListLength(rightList) )
    {
        freeList(&leftList);
        freeList(&rightList);

        return 0;
    }

    leftIter  = leftList;
    rightIter = rightList;
    while ( leftIter != NULL )
    {
        if ( ( leftIter->data.val == missingVal
               || rightIter->data.val == missingVal )
             && leftIter->data.val != rightIter->data.val )
            break;

        leftIter  = leftIter->next;
        rightIter = rightIter->next;
    }

    freeList(&leftList);
    freeList(&rightList);

    return NULL == leftIter;
}

/* In this approach, we maintain a two lists:
 * - smaller is a list of values that visit node value must be smaller than any value in the list
 * - larger is a list of values that visit node value must be larger than any value in the list
 *
 * The other approach is to traverse the whole tree in in order list a
 * and passing last visit value from node to node and where new node value should not exceed last
 * visit node if it is a binary search tree.
 */
int isTreeSearchTreeRecursive(struct TreeNode *root,
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

    pushStackInt(root->val, &smaller);
    if ( ! isTreeSearchTreeRecursive(root->left, smaller, larger) )
        return 0;

    tmp = popStack(&smaller);
    free(tmp);

    pushStackInt(root->val, &larger);
    if ( ! isTreeSearchTreeRecursive(root->right, smaller, larger) )
        return 0;

    tmp = popStack(&larger);
    free(tmp);

    return 1;
}


int isTreeSearchTree(struct TreeNode *root)
{
    struct ListNode *smaller = NULL;
    struct ListNode *larger  = NULL;
    int              ret;


    ret = isTreeSearchTreeRecursive(root, smaller, larger);

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

    enQueueInt(root->val, start);

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

    enQueueInt(root->val, start);

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

    enQueueInt(root->val, start);
}


struct ListNode * getPostOrderList(struct TreeNode *root)
{
    struct ListNode *start = NULL;


    getPostOrderListRecursively(root, &start);

    return start;
}


int isTreeSubTree(struct TreeNode *tree1, struct TreeNode *tree2)
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

    ret = isSubsetList(list1, list2 );

cleanup:
    freeList(&list1);
    freeList(&list2);

    return ret;
}


struct TreeNode * findTreeNodeParent(struct TreeNode *root,
                                     struct TreeNode *node)
{
    struct TreeNode *parent = NULL;


    if ( NULL == root
         || NULL == node )
        return NULL;

    if ( root == node )
        return node;

    if ( root->left == node
         || root->right == node )
        return root;

    if ( NULL != root->left )
        parent = findTreeNodeParent(root->left, node);

    if ( NULL == parent)
    {
        if ( NULL != root->right )
        {
            parent = findTreeNodeParent(root->right, node);
        }
    }

    return parent;
}


static struct TreeNode * treeMirrorSwapRecursive(struct TreeNode *root)
{
    struct TreeNode *tmp;


    if ( NULL == root )
       return NULL;

    tmp         = root->left;
    root->left  = treeMirrorSwapRecursive(root->right);
    root->right = treeMirrorSwapRecursive(tmp);

    return root;
}


void treeMirrorSwap(struct TreeNode *root)
{
    treeMirrorSwapRecursive(root);
}


/* The logic is simple:
 * - we can build a binary tree from post order list much easily via a reverse postorder tree it resembles the
 *   top-down fashion of the tree with right branch lean. so a binary tree postorder is left-lean, a reversed
 *   postorder list is right branch lean.
 *
 * - if a post order node value appears on right position of the in order list, then the value is part of the
 *   right branch of previous node.
 *
 * - if a post order node value appears on left positio of the in order list, then the value is part of the
 *   left branch of _parent_ node of previous node.
 */
struct TreeNode * buildBinaryTree(struct ListNode *inorder, struct ListNode *postorder)
{
    int              cnt;
    int              i;
    int              j;
    struct TreeNode *root;
    struct TreeNode *new;
    struct TreeNode *prev;


    if ( NULL == inorder
         || NULL == postorder )
        return NULL;

    cnt = getListLength(inorder);
    if ( cnt != getListLength(postorder) )
        return NULL;

    root      = NULL;
    postorder = reverseQueue(postorder);
    i         = 0;
    while ( NULL != postorder )
    {
        new = malloc(sizeof( struct TreeNode ));

        new->left  = NULL;
        new->right = NULL;
        new->val   = postorder->data.val;

        if ( NULL == root )
        {
            root = new;
            prev = new;
        }
        else
        {
            j = findListNodeIntIndex(inorder, new->val);
            assert( j != -1 ); // we do not handle it properly yet.

            if ( j >= i )
            {
                prev->right = new;
                prev        = new;
            }
            else
            {
                prev       = findTreeNodeParent(root, prev);
                prev->left = new;
            }
        }

        postorder = postorder->next;
        i++;
    }

    return root;
}


void traverseTreeNodeInLevelLeftToRightOrderRecursive(struct ListNode             **firstLevel,
                                                      struct ListNode             **nextLevel,
                                                      int                           level,
                                                      int                           pos,
                                                      bTreeLevelTraversalCallback   func,
                                                      void                         *data)
{
    struct ListNode *lnode;
    struct ListNode *liter;
    struct TreeNode *tnode;
    int              stop;


    stop  = 0;
    lnode = deQueue(firstLevel);
    while ( NULL != lnode )
    {
        tnode = lnode->data.ref;
        free(lnode);

        func(tnode, pos, level, &stop, data);
        if ( stop )
           goto quit;

        pos = pos + 1;

        lnode = deQueue(firstLevel);
    }

    assert(*firstLevel == NULL);

    *firstLevel = *nextLevel;
    *nextLevel  = NULL;

    liter = *firstLevel;
    while ( NULL != liter )
    {
        tnode = liter->data.ref;

        if ( NULL != tnode->left )
            enQueueRef(tnode->left, nextLevel);

        if ( NULL != tnode->right )
            enQueueRef(tnode->right, nextLevel);

        liter = liter->next;
    }

    if ( NULL != *firstLevel )
    {
       level = level + 1;

       traverseTreeNodeInLevelLeftToRightOrderRecursive(firstLevel, nextLevel, level, pos, func, data);
    }

quit:
    freeQueue(firstLevel);
    freeQueue(nextLevel);
}


void traverseTreeNodeInLevelLeftToRightOrder(struct TreeNode *root, bTreeLevelTraversalCallback func, void *data)
{
    struct ListNode *firstLevel = NULL;
    struct ListNode *nextLevel  = NULL;
    int              level      = 0;
    int              pos        = 0;


    if ( NULL == root )
        return;

    enQueueRef(root, &firstLevel);

    if ( NULL != root->left )
        enQueueRef(root->left, &nextLevel);

    if ( NULL != root->right )
        enQueueRef(root->right, &nextLevel);

    traverseTreeNodeInLevelLeftToRightOrderRecursive(&firstLevel,
                                                     &nextLevel,
                                                     level,
                                                     pos,
                                                     func,
                                                     data);
}
