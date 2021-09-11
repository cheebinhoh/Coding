/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Find the tree maximum level.
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"


/* To derive the max level of tree, we can use any of the depth first search, and increase
 * the level as we progress to left or right branch, and only return the max level if the
 * level we are now in is higher than current max level value.
 */
void treeMaxLevelInternal(struct TreeNode *root,
                          int              level,
                          int             *maxLevel)
{
    if ( NULL == root )
        return;


    level++;

    if ( level > *maxLevel )
        *maxLevel = level;

    if ( NULL != root->left )
        treeMaxLevelInternal(root->left, level, maxLevel);

    if ( NULL != root->right )
        treeMaxLevelInternal(root->right, level, maxLevel);

    return;
}


/* An API front end to return max level of tree
 */
int treeMaxLevel(struct TreeNode *root)
{
    int level = 0;


    treeMaxLevelInternal(root, level, &level);

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
    struct TreeNode *root  = NULL;
    struct TreeNode *other = NULL;


    root        = malloc(sizeof( struct TreeNode ));
    root->val   = 0;
    root->left  = NULL;
    root->right = NULL;

    other        = malloc(sizeof( struct TreeNode ));
    other->val   = 1;
    other->left  = NULL;
    other->right = NULL;
    root->left   = other;

    other            = malloc(sizeof( struct TreeNode ));
    other->val       = 3;
    other->left      = NULL;
    other->right     = NULL;
    root->left->left = other;

    other             = malloc(sizeof( struct TreeNode ));
    other->val        = 4;
    other->left       = NULL;
    other->right      = NULL;
    root->left->right = other;

    other        = malloc(sizeof( struct TreeNode ));
    other->val   = 2;
    other->left  = NULL;
    other->right = NULL;
    root->right  = other;

    other              = malloc(sizeof( struct TreeNode ));
    other->val         = 5;
    other->left        = NULL;
    other->right       = NULL;
    root->right->right = other;

    other                    = malloc(sizeof( struct TreeNode ));
    other->val               = 6;
    other->left              = NULL;
    other->right             = NULL;
    root->right->right->left = other;

    other                           = malloc(sizeof( struct TreeNode ));
    other->val                      = 7;
    other->left                     = NULL;
    other->right                    = NULL;
    root->right->right->left->right = other;

    other                     = malloc(sizeof( struct TreeNode ));
    other->val                = 8;
    other->left               = NULL;
    other->right              = NULL;
    root->right->right->right = other;

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    printf("Max tree level = %d\n", treeMaxLevel(root));

    return 0;
 }
