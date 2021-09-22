/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Find out if there exists a path from root to a leaf that its sum of values of all nodes
 * equal to the target.
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"


struct NextNodeOnSameLevel
{
    int              val;
    int              visited;
    struct TreeNode *nextNode;
};


void levelTraversalCallback(struct TreeNode *node, int pos, int level, int *stop, void *data)
{
    printf("---- level = %d, pos = %d, val = %d\n", level, pos, node->val);
}


void nextNodeOnSameLevelCallback(struct TreeNode *node, int pos, int level, int *stop, void *data)
{
    struct NextNodeOnSameLevel *cbData = data;


    if ( cbData->visited )
    {
        cbData->nextNode = node;
        *stop            = 1;
    }
    else
    {
        cbData->visited = node->val == cbData->val;
    }
}


void findNextNodeOnSameLevel(struct TreeNode *root, int val)
{
    struct NextNodeOnSameLevel data;


    data.visited  = 0;
    data.val      = val;
    data.nextNode = NULL;

    traverseTreeNodeInLevelLeftToRightOrder(root, nextNodeOnSameLevelCallback, &data);
    if ( NULL != data.nextNode )
    {
        printf("The next node of %d is %d\n", val, data.nextNode->val);
    }
    else
    {
        printf("The next node of %d is none\n", val);
    }
}


struct NodesOnReversedOrder
{
    int              level;
    struct ListNode *nodeList;
    struct ListNode *tailList;
};


void printTreeNodeOnReverseOrderLevelCallback(struct TreeNode *node, int pos, int level, int *stop, void *data)
{
    struct NodesOnReversedOrder *pData = data;
    struct ListNode             *iter;


    if ( pData->level != level )
    {
        iter = pData->tailList;
        while ( NULL != iter->next )
            iter = iter->next;

        iter->next = pData->nodeList;

        pData->nodeList = pData->tailList;
        pData->tailList = NULL;
        pData->level    = level;
    }

    enQueueRef(node, &(pData->tailList));
}


void printTreeNodeOnReverseOrderLevel(struct TreeNode *root)
{
    struct NodesOnReversedOrder  data;
    struct ListNode             *iter;
    struct TreeNode             *node;

    data.level    = 0;
    data.nodeList = NULL;
    data.tailList = NULL;

    traverseTreeNodeInLevelLeftToRightOrder(root, printTreeNodeOnReverseOrderLevelCallback, &data);

    iter = data.tailList;
    while ( NULL != iter )
    {
        node = iter->data.ref;
        printf("%d ", node->val);

        iter = iter->next;
    }

    iter = data.nodeList;
    while ( NULL != iter )
    {
        node = iter->data.ref;
        printf("%d ", node->val);

        iter = iter->next;
    }

    if ( NULL != data.tailList )
        printf("\n");

    freeList(&(data.tailList));
    freeList(&(data.nodeList));
}


struct FindLargestBST
{
    int size;
    int level;
};


void traversalToFindLargestBST(struct TreeNode *node, int pos, int level, int *stop, void *data)
{
    struct FindLargestBST *pData = data;


    if ( pData->size > 0 && pData->level != level )
    {
        *stop = 1;
    }
    else
    {
        if ( isTreeSearchTree(node) )
        {
            pData->size = findTotalNumberOfNodes(node);
        }
    }

    pData->level = level;
}


void printTheLargestBSTInBinaryTree(struct TreeNode *root)
{
    struct FindLargestBST data;


    data.size  = 0;
    data.level = 0;

    traverseTreeNodeInLevelLeftToRightOrder(root, traversalToFindLargestBST, &data);
    printf("the largest BST in binary tree is %d\n", data.size);
}


void traversalInTreeBoundary(struct TreeNode *node, int pos, int *stop, void *data)
{
    printf("%d ", node->val);
}


void traversalInVerticalAxisLevel(struct TreeNode *node, int pos, int axis, int *stop, void *data)
{
     int *lastAxis;


     lastAxis = data;
     if ( axis != *lastAxis )
     {
         printf("\n");
         *lastAxis = axis;
     }

     printf("%d ", node->val);
}


void printNodeInVerticalAxisLevel(struct TreeNode *root)
{
    int i;
    int axis;


    i    = 1;
    axis = i;
    while ( i > 0 )
    {
        axis = i;
        i = i << 1;
        i = i + 1;
    }

    traverseTreeNodeInVerticalOrderTopDown(root, traversalInVerticalAxisLevel, &axis);
    printf("\n");
}


void printNodeInDiagonalAxisLevel(struct TreeNode *root)
{
    int i;
    int axis;


    i    = 1;
    axis = i;
    while ( i > 0 )
    {
        axis = i;
        i = i << 1;
        i = i + 1;
    }

    traverseTreeNodeInDiagonalWithAxisLevel(root, traversalInVerticalAxisLevel, &axis);
    printf("\n");
}


int main(int argc, char * argv[])
{
    int              level = 0;
    struct TreeNode *root  = NULL;
    struct TreeNode *node  = NULL;
    struct TreeNode *other = NULL;


    printf("Test 1: is tree a binary search tree\n");
    printf("\n");

    root = addTreeNodeAndRebalanceTree(root, 1);
    root = addTreeNodeAndRebalanceTree(root, 2);
    root = addTreeNodeAndRebalanceTree(root, 3);
    root = addTreeNodeAndRebalanceTree(root, 4);
    root = addTreeNodeAndRebalanceTree(root, 6);
    root = addTreeNodeAndRebalanceTree(root, 7);
    root = addTreeNodeAndRebalanceTree(root, 8);
    root = addTreeNodeAndRebalanceTree(root, 9);

    other        = malloc(sizeof(struct TreeNode));
    other->val   = -2;
    other->left  = NULL;
    other->right = NULL;

    node       = findTreeNode(root, 1);
    node->val  = -1;
    node->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 0;
    other->left  = NULL;
    other->right = NULL;
    node->right  = other;

    printf("The tree topology:\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");


    root = delTreeNode(root, 2);
    root = delTreeNode(root, 7);
    printf("After deleting 2 and 7, the tree topology:\n");
    printTreeNodeInTreeTopology(root);

    if ( isTreeSearchTree(root) )
    {
        printf("\n");
        printf("It is still a binary search tree\n");
    }

    printf("\n");
    printf("\n");

    printf("Test 2: traverse tree in level left to right\n");
    traverseTreeNodeInLevelLeftToRightOrder(root, levelTraversalCallback, NULL);
    printf("\n");


    printf("\n");
    printf("Test 3: find the next node of others in same level\n");
    findNextNodeOnSameLevel(root, 0);
    findNextNodeOnSameLevel(root, 6);
    findNextNodeOnSameLevel(root, -2);
    printf("\n");
    printf("\n");

    printf("Test 4: print values in reversed order of level\n");
    printTreeNodeOnReverseOrderLevel(root);
    printf("\n");

    root        = malloc(sizeof(struct TreeNode));
    root->val   = 50;
    root->left  = NULL;
    root->right = NULL;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 30;
    other->left  = NULL;
    other->right = NULL;

    root->left   = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 5;
    other->left  = NULL;
    other->right = NULL;

    root->left->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 20;
    other->left  = NULL;
    other->right = NULL;

    root->left->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 60;
    other->left  = NULL;
    other->right = NULL;

    root->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 45;
    other->left  = NULL;
    other->right = NULL;

    root->right->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 70;
    other->left  = NULL;
    other->right = NULL;

    root->right->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 65;
    other->left  = NULL;
    other->right = NULL;

    root->right->right->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 80;
    other->left  = NULL;
    other->right = NULL;

    root->right->right->right = other;

    printf("\n");
    printf("The tree topology:\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");

    printf("Test 5: total number of nodes in tree is %d\n", findTotalNumberOfNodes(root));
    printf("\n");
    printf("\n");
    printf("Test 6: ");
    printTheLargestBSTInBinaryTree(root);
    printf("\n");
    printf("\n");

    printf("Test 7: traverse node in tree boundary left to right\n");
    printf("\n");
    printf("The tree topology:\n");

    root = malloc(sizeof(struct TreeNode));
    root->val   = 10;
    root->left  = NULL;
    root->right = NULL;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 5;
    other->left  = NULL;
    other->right = NULL;

    root->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 8;
    other->left  = NULL;
    other->right = NULL;

    root->left->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 9;
    other->left  = NULL;
    other->right = NULL;

    root->left->right->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 2;
    other->left  = NULL;
    other->right = NULL;

    root->left->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 4;
    other->left  = NULL;
    other->right = NULL;

    root->left->left->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 3;
    other->left  = NULL;
    other->right = NULL;

    root->left->left->right->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 6;
    other->left  = NULL;
    other->right = NULL;

    root->left->left->right->left->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 7;
    other->left  = NULL;
    other->right = NULL;

    root->left->left->right->left->right->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 15;
    other->left  = NULL;
    other->right = NULL;

    root->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 14;
    other->left  = NULL;
    other->right = NULL;

    root->right->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 13;
    other->left  = NULL;
    other->right = NULL;

    root->right->left->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 16;
    other->left  = NULL;
    other->right = NULL;

    root->right->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 18;
    other->left  = NULL;
    other->right = NULL;

    root->right->right->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 19;
    other->left  = NULL;
    other->right = NULL;

    root->right->right->left->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 17;
    other->left  = NULL;
    other->right = NULL;

    root->right->right->right = other;
    printTreeNodeInTreeTopology(root);
    printf("\n");

    printf("node values: ");
    traverseTreeNodeInBoundary(root, traversalInTreeBoundary, NULL);
    printf("\n");
    printf("\n");

    root        = malloc(sizeof(struct TreeNode));
    root->val   = 1;
    root->left  = NULL;
    root->right = NULL;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 2;
    other->left  = NULL;
    other->right = NULL;

    root->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 3;
    other->left  = NULL;
    other->right = NULL;

    root->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 6;
    other->left  = NULL;
    other->right = NULL;

    root->right->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 5;
    other->left  = NULL;
    other->right = NULL;

    root->right->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 7;
    other->left  = NULL;
    other->right = NULL;

    root->right->left->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 8;
    other->left  = NULL;
    other->right = NULL;

    root->right->left->right = other;

    printf("\n");
    printf("Test 8: traverse node in vertical axis left to right\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");
    printf("Nodes values (an axis per row):\n");
    printNodeInVerticalAxisLevel(root);

    printf("\n");
    printf("\n");

    printf("Test 9: Find the diameter of a binary tree\n");
    root        = malloc(sizeof(struct TreeNode));
    root->val   = 1;
    root->left  = NULL;
    root->right = NULL;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 2;
    other->left  = NULL;
    other->right = NULL;

    root->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 4;
    other->left  = NULL;
    other->right = NULL;

    root->left->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 3;
    other->left  = NULL;
    other->right = NULL;

    root->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 5;
    other->left  = NULL;
    other->right = NULL;

    root->right->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 7;
    other->left  = NULL;
    other->right = NULL;

    root->right->left->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 8;
    other->left  = NULL;
    other->right = NULL;

    root->right->left->right = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 6;
    other->left  = NULL;
    other->right = NULL;

    root->right->right = other;

    printf("The tree topology:\n");
    printf("\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");
    printf("The tree diameter is %d\n", findMaximumDiameter(root));
    printf("\n");

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 9;
    other->left  = NULL;
    other->right = NULL;

    root->right->right->right = other;
    root->left = NULL;

    printf("Adjusting the tree, the new topology:\n");
    printf("\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");
    printf("The tree diameter is %d\n", findMaximumDiameter(root));
    printf("\n");

    printf("\n");
    printf("Test 10: Print diagonal traversal of a binary tree\n");

    root = malloc(sizeof(struct TreeNode));
    root->val   = 1;
    root->left  = NULL;
    root->right = NULL;

    other = malloc(sizeof(struct TreeNode ));
    other->val   = 2;
    other->left  = NULL;
    other->right = NULL;

    root->left = other;

    other = malloc(sizeof(struct TreeNode ));
    other->val   = 4;
    other->left  = NULL;
    other->right = NULL;

    root->left->left = other;

    other = malloc(sizeof(struct TreeNode ));
    other->val   = 3;
    other->left  = NULL;
    other->right = NULL;

    root->right = other;

    other = malloc(sizeof(struct TreeNode ));
    other->val   = 5;
    other->left  = NULL;
    other->right = NULL;

    root->right->left = other;

    other = malloc(sizeof(struct TreeNode ));
    other->val   = 7;
    other->left  = NULL;
    other->right = NULL;

    root->right->left->left = other;

    other = malloc(sizeof(struct TreeNode ));
    other->val   = 6;
    other->left  = NULL;
    other->right = NULL;

    root->right->right = other;

    other = malloc(sizeof(struct TreeNode ));
    other->val   = 8;
    other->left  = NULL;
    other->right = NULL;

    root->right->left->right = other;

    printf("\n");
    printf("The tree topology:\n");
    printTreeNodeInTreeTopology(root);

    printNodeInDiagonalAxisLevel(root);
    printf("\n");
    printf("\n");

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    return 0;
 }
