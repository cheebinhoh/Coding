/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Depth first search, preorder, inorder and postorder, both recursive and iterative way
 */

#include <stdio.h>
#include <stdlib.h>


/* Definition for a binary tree node.
 */
struct TreeNode {

     int val;
     struct TreeNode *left;
     struct TreeNode *right;
};


void treeMaxLevelInternal(struct TreeNode *root,
                          int              level,
                          int             *maxLevel)
{
    if ( NULL == root )
    {
        return;
    }
    else
    { 
        level++;

	if ( level > *maxLevel )
        {
            *maxLevel = level;
        }

        if ( NULL != root->left )
        {
            treeMaxLevelInternal(root->left, level, maxLevel);
        }       

        if ( NULL != root->right )
        {
           treeMaxLevelInternal(root->right, level, maxLevel);
        }

        return;
    }
}


int treeMaxLevel(struct TreeNode *root)
{
    int level = 0;

    treeMaxLevelInternal(root, 0, &level);

    return level; 
}


/*
 *               0
 *               |
 *      +--------+---------+
 *      1                  2
 *      |                  |
 * +----+----+       //----+----+
 * 3         4                  5
 *                              |
 *                    +---------+---------+
 *                    6                   8
 *                    |                   |
 *              //----+----+          //--+--//
 *                         7 
 */
int main(int argc, char * argv[])
{
    struct TreeNode *root = NULL;
    struct TreeNode *other = NULL;

    root = malloc(sizeof( struct TreeNode));
    root->val = 0;
    root->left = root->right = NULL;

    other = malloc(sizeof( struct TreeNode ));
    other->val = 1;
    other->left = other->right = NULL;
    root->left = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val = 3;
    other->left = other->right = NULL;
    root->left->left = other;
 
    other = malloc(sizeof( struct TreeNode ));
    other->val = 4;
    other->left = other->right = NULL;
    root->left->right = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val = 2;
    other->left = other->right = NULL;
    root->right = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val = 5;
    other->left = other->right = NULL;
    root->right->right = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val = 6;
    other->left = other->right = NULL;
    root->right->right->left = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val = 7;
    other->left = other->right = NULL;
    root->right->right->left->right = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val = 8;
    other->left = other->right = NULL;
    root->right->right->right = other;

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    printf("Max tree level = %d\n", treeMaxLevel(root));

    return 0;
 }

