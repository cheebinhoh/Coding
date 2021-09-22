/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Depth first search, preorder, inorder and postorder, both recursive and iterative way.
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"
#include "btreetraverse.h"


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

    printf("The tree topology:\n");
    printf("\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");

    printf("pre order  = ");
    preOrderTraversal(root);

    printf("post order = ");
    postOrderTraversal(root);

    printf("in order   = ");
    inOrderTraversal(root);
    printf("\n");

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    return 0;
 }
