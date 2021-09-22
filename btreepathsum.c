/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Find out if there exists a path from root to a leaf that its sum of values of all nodes
 * equal to the target.
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"


/* This is internal recursive method that we sum up all value until leaf and then compare the sum
 * value to target sum and return 0 (false) or 1 (true) that the sum value is equal to target value.
 *
 * we only call btreePathSumInternal on a right branch if left branch does not result in 1.
 */
int btreePathSumInternal(struct TreeNode *root,
                         int              sum,
                         int              targetSum)
{
    int ret = 0;


    if ( NULL == root )
        return 0; // when tree is empty, there is no path, a path must be at least a node in minimum

    sum += root->val;

    if ( NULL != root->left )
        ret = btreePathSumInternal(root->left,
                                   sum,
                                   targetSum);

    if ( 0 == ret )
    {
        if ( NULL != root->right )
            ret = btreePathSumInternal(root->right, sum, targetSum);
        else
            ret = sum == targetSum;
    }

    return ret;
}


/* This is API facing method to check if there is a path of sum of value of nodes that matches the target value.
 */
int btreePathSum(struct TreeNode *root,
                 int              targetSum)
{
    int sum = 0;


    return btreePathSumInternal(root, sum, targetSum);
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
    printf("target sum (15) is %d\n", btreePathSum(root, 15));
    printf("target sum (6) is %d\n", btreePathSum(root, 6));
    printf("target sum (5) is %d\n", btreePathSum(root, 5));
    printf("target sum (7) is %d\n", btreePathSum(root, 7));
    printf("\n");

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    return 0;
 }
