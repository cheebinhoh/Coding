/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Find the maximum sum path betweeen two leaves in a binary tree.
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"


int findMaxinumSumPathBetween2Leaves(struct TreeNode *root, int val1, int val2)
{
    int sum1;
    int sum2;


    root = findLeastCommonAncestor(root, val1, val2);
    if ( NULL == root )
        return 0;

    sum1 = findMaximumSumPathToLeafValue(root, val1);
    sum2 = findMaximumSumPathToLeafValue(root, val2);

    return sum1 + sum2 - root->val;
}


int main(int argc, char * argv[])
{
    struct TreeNode *root  = NULL;
    struct TreeNode *other = NULL;


    root = malloc(sizeof( struct TreeNode ));
    root->val   = 0;
    root->left  = NULL;
    root->right = NULL;

    other = malloc(sizeof( struct TreeNode ));
    other->val   = 1;
    other->left  =  NULL;
    other->right = NULL;
    root->left   = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val       = 3;
    other->left      = NULL;
    other->right     = NULL;
    root->left->left = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val        = 4;
    other->left       = NULL;
    other->right      = NULL;
    root->left->right = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val   = 2;
    other->left  = NULL;
    other->right = NULL;
    root->right  = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val         = 5;
    other->left        = NULL;
    other->right       = NULL;
    root->right->right = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val               = 6;
    other->left              = NULL;
    other->right             = NULL;
    root->right->right->left = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val                      = 7;
    other->left                     = NULL;
    other->right                    = NULL;
    root->right->right->left->right = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val                 = 8;
    other->left                = NULL;
    other->right               = NULL;
    root->right->right->right  = other;

    printf("The tree topology:\n");
    printf("\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");

    printf("The maximum sum path between leaves %d and %d is %d\n",
           3, 4, findMaxinumSumPathBetween2Leaves(root, 3, 4));
    printf("The maximum sum path between leaves %d and %d is %d\n",
           8, 7, findMaxinumSumPathBetween2Leaves(root, 8, 7));
    printf("\n");
    printf("\n");

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    return 0;
 }
