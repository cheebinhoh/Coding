/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Check if tree is sysmetric that right branch of root is mirror of left branch.
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"


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


    other        = malloc(sizeof( struct TreeNode ));
    other->val   = 1;
    other->left  = NULL;
    other->right = NULL;
    root->right  = other;


    other        = malloc(sizeof( struct TreeNode ));
    other->val   = 2;
    other->left  = NULL;
    other->right = NULL;

    root->right->right = other;


    other        = malloc(sizeof( struct TreeNode ));
    other->val   = 2;
    other->left  = NULL;
    other->right = NULL;

    root->left->left = other;


    other        = malloc(sizeof( struct TreeNode ));
    other->val   = 3;
    other->left  = NULL;
    other->right = NULL;

    root->left->right = other;


    other        = malloc(sizeof( struct TreeNode ));
    other->val   = 3;
    other->left  = NULL;
    other->right = NULL;

    root->right->left = other;

    printf("The tree topology:\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");

    printf("Is sysmetric = %d\n", isTreeSymmetric(root));
    printf("\n");

    root->right->left = NULL;

    printf("The tree topology (after setting root->right->left to NULL):\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");

    printf("Is sysmetric = %d\n", isTreeSymmetric(root));
    printf("\n");

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    return 0;
 }
