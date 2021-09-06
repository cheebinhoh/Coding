/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Binary tree utility
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"


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
        node = malloc(sizeof( struct TreeNode )); // I do not care about error as it is just a test

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

    if ( ( rightLevel - leftLevel ) >= 2 )
    {
        newRoot = root->right;

        if ( NULL != root->right->left )
        {
            newRoot = root->right->left;
            root->right->left = NULL;
            newRoot->right = root->right;
        }

        root->right = NULL;
        newRoot->left = root;

        *parent = newRoot;
    }
    else if ( ( leftLevel - rightLevel ) >= 2 )
    {
        newRoot = root->left;

        if ( NULL != root->left->right )
        {
            newRoot = root->left->right;
            root->left->right = NULL;
            newRoot->left = root->left;
        }

        root->left = NULL;
        newRoot->right = root;

        *parent = newRoot;
    }
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
