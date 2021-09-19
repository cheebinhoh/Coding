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
}

int main(int argc, char * argv[])
{
    int              level = 0;
    struct TreeNode *root  = NULL;
    struct TreeNode *node  = NULL;
    struct TreeNode *other = NULL;


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

    node = findTreeNode(root, 1);
    node->val  = -1;
    node->left = other;

    other        = malloc(sizeof(struct TreeNode));
    other->val   = 0;
    other->left  = NULL;
    other->right = NULL;
    node->right  = other;

    printf("The tree topology:\n");
    printTreeNodeInTreeTopology(root);

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

    printf("Traverse tree in level left to right\n");
    traverseTreeNodeInLevelLeftToRightOrder(root, levelTraversalCallback, NULL);
    printf("\n");


    printf("\n");
    printf("Find the next node of others in same level\n");
    findNextNodeOnSameLevel(root, 0);
    findNextNodeOnSameLevel(root, 6);
    printf("\n");

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    return 0;
 }
