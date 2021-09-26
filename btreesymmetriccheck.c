/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Check if tree is sysmetric that right branch of root is mirror of left branch
 * in layout of the tree (not the values of node).
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"


void checkIfTreeIsSysmetric(struct TreeNode *root)
{
    if ( isTreeSymmetric(root) )
    {
        printf("The tree is sysmetric\n");
    }
    else
    {
        printf("The tree is not sysmetric\n");
    }
}


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
    printf("\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");

    checkIfTreeIsSysmetric(root);
    printf("\n");
    printf("\n");

    root->right->left = NULL;

    printf("The tree topology (after setting root->right->left to NULL):\n");
    printf("\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");

    checkIfTreeIsSysmetric(root);
    printf("\n");
    printf("\n");

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.


    return 0;
 }
