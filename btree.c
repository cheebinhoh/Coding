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
