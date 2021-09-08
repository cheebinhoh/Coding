/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * ADT Binary tree (with AVL self-balancing functionality)
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"


static int debug = 0;


int isTreeIdentical(struct TreeNode *left, struct TreeNode *right)
{
    if ( NULL == left
         || NULL == right )
    {
       return left == right;
    }


    return left->val == right->val
           && isTreeIdentical(left->left, right->left)
           && isTreeIdentical(left->right, right->right);
}

struct TreeNode * mirrorSwap(struct TreeNode *root)
{
    struct TreeNode *tmp;


    if ( NULL == root )
       return NULL;


    tmp         = root->left;
    root->left  = mirrorSwap(root->right);
    root->right = mirrorSwap(tmp);

    return root;
}


void printTreeNodeInOrder(struct TreeNode *root)
{
    if ( NULL == root )
        return;


    printTreeNodeInOrder(root->left);
    printf("%d, ", root->val);
    printTreeNodeInOrder(root->right);
}


struct TreeNode * findTreeNode(struct TreeNode *root, int val)
{
    if ( NULL == root )
        return NULL;


    if ( val == root->val )
        return root;
    else if ( val < root->val )
        return findTreeNode(root->left, val);
    else
        return findTreeNode(root->right, val);
}

struct TreeNode * addTreeNodeAndRebalanceTree(struct TreeNode *root, int val)
{
    root = addTreeNode(root, val);

    return treeRebalance(root);
}

struct TreeNode * addTreeNode(struct TreeNode *root, int val)
{
    struct TreeNode *node;


    if ( NULL == root )
    {
        node = malloc(sizeof( struct TreeNode )); // I do not care about NULL error as it is just a test

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

int determineMaxDepthLevel(struct TreeNode *root,
                           int              level)
{
    int rightLevel;
    int leftLevel;


    if ( NULL == root )
        return level;

    rightLevel = determineMaxDepthLevel(root->left, level + 1);
    leftLevel = determineMaxDepthLevel(root->right, level + 1);

    return ( rightLevel > leftLevel ) ? rightLevel : leftLevel;
}

void treeRebalanceRecursive(struct TreeNode **parent,
                            struct TreeNode  *root)
{
    int              leftLevel;
    int              rightLevel;
    struct TreeNode *newRoot;


    if ( NULL == root )
       return;

    if ( NULL != root->left )
       treeRebalanceRecursive(&(root->left), root->left);

    if ( NULL != root->right )
       treeRebalanceRecursive(&(root->right), root->right);

    rightLevel = determineMaxDepthLevel(root->right, 0);
    leftLevel = determineMaxDepthLevel(root->left, 0);

    newRoot = root;

    if ( ( rightLevel - leftLevel ) >= 2 )
    {
        newRoot = root->right;

        rightLevel = determineMaxDepthLevel(newRoot->right, 0);
        leftLevel = determineMaxDepthLevel(newRoot->left, 0);

        if ( leftLevel > rightLevel )
        {
            newRoot = newRoot->left;
            if ( NULL != newRoot->left )
                newRoot->left->left = root;
            else
                newRoot->left = root;

            if ( NULL != newRoot->right )
                newRoot->right->right = root->right;
            else
                newRoot->right = root->right;

            root->right->left = NULL;
        }
        else if ( rightLevel > leftLevel )
        {
            if ( NULL != newRoot->left )
                newRoot->left->left = root;
            else
                newRoot->left = root;
        }
        else
        {
            newRoot->right = root;
        }

        root->right = NULL;
    }
    else if ( ( leftLevel - rightLevel ) >= 2 )
    {
        newRoot = root->left;

        rightLevel = determineMaxDepthLevel(newRoot->right, 0);
        leftLevel = determineMaxDepthLevel(newRoot->left, 0);

        if ( rightLevel > leftLevel )
        {
            newRoot = newRoot->right;
            if ( NULL != newRoot->right )
                newRoot->right->right = root;
            else
                newRoot->right = root;

            if ( NULL != newRoot->left )
                newRoot->left->left = root->left;
            else
                newRoot->left = root->left;

            root->left->right = NULL;
        }
        else if ( leftLevel > rightLevel )
        {
            if ( NULL != newRoot->right )
                newRoot->right->right = root;
            else
                newRoot->right = root;
        }
        else
        {
            newRoot->left = root;
        }

        root->left = NULL;
    }

    *parent = newRoot;
}

struct TreeNode * treeRebalance(struct TreeNode *root)
{
    if ( NULL == root )
        return NULL;


    treeRebalanceRecursive(&root, root);

    return root;
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
           determineMaxDepthLevel(root->left, 0),
           determineMaxDepthLevel(root->right, 0));

    printTreeNodeInTreeTopologyRecursive(root->left, level + 1);
    printTreeNodeInTreeTopologyRecursive(root->right, level + 1);
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
