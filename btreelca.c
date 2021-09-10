/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Find the least common ancestor of two nodes.
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"
#include "btreetraverse.h"


struct TreeNode * findNode(struct TreeNode *root,
                           int              val)
{
    struct TreeNode *node;


    if ( NULL == root )
        return NULL;

    if ( val == root->val )
        return root;

    node = findNode(root->left, val);
    if ( NULL != node )
        return node;

    node = findNode(root->right, val);

    return node;
}

/* Internal recursive method to find the least common ancestor
 */
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
             && findNode(root->right, val2) != NULL )
        {
            return root;
        }

        if ( root->val == val2
             && findNode(root->right, val1) != NULL )
        {
            return root;
        }

        return findLeastCommonAncestorInternal(root->right, val1, val2);
    }

    if ( NULL == root->right )
    {
        if ( root->val == val1
             && findNode(root->left, val2) != NULL )
        {
            return root;
        }

        if ( root->val == val2
             && findNode(root->left, val1) != NULL )
        {
            return root;
        }

        return findLeastCommonAncestorInternal(root->left, val1, val2);
    }

    if ( findNode(root->left, val1) != NULL
         && findNode(root->right, val2) != NULL )
    {
        return root;
    }

    if ( findNode(root->left, val2) != NULL
         && findNode(root->right, val1) != NULL )
    {
        return root;
    }

    node = findLeastCommonAncestorInternal(root->left, val1, val2);
    if ( NULL != node )
    {
        return node;
    }

    node = findLeastCommonAncestorInternal(root->right, val1, val2);
    if ( NULL != node )
    {
        return node;
    }

    return NULL;
}

/* API front end to find least common ancestor
 */
struct TreeNode * findLeastCommonAncestor(struct TreeNode *root,
                                          int              val1,
                                          int              val2)
{
    return findLeastCommonAncestorInternal(root, val1, val2);
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


    root = malloc(sizeof( struct TreeNode ));
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


    other = findLeastCommonAncestor(root, 1, 2);
    if ( NULL != other )
    {
        printf("The least common ancestor of 1 and 2 is %d\n", other->val);
    }

    other = findLeastCommonAncestor(root, 3, 4);
    if ( NULL != other )
    {
        printf("The least common ancestor of 3 and 4 is %d\n", other->val);
    }

    other = findLeastCommonAncestor(root, 2, 5);
    if ( NULL != other )
    {
        printf("The least common ancestor of 2 and 5 is %d\n", other->val);
    }

    other = findLeastCommonAncestor(root, 2, 8);
    if ( NULL != other )
    {
        printf("The least common ancestor of 2 and 8 is %d\n", other->val);
    }

    other = findLeastCommonAncestor(root, 1, 8);
    if ( NULL != other )
    {
        printf("The least common ancestor of 1 and 8 is %d\n", other->val);
    }

    other = findLeastCommonAncestor(root, 3, 8);
    if ( NULL != other )
    {
        printf("The least common ancestor of 3 and 8 is %d\n", other->val);
    }

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    return 0;
 }
