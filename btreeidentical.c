/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Given the roots of two binary trees, determine if these trees are
 * identical or not. Identical trees have the same layout and data at each node.
 */

#include <stdio.h>
#include "btree.h"


int isTreeIdentical(struct TreeNode *left, struct TreeNode *right)
{
    if ( NULL == left
         || NULL == right )
       return left == right;

    return left->val == right->val
           && isTreeIdentical(left->left, right->left)
           && isTreeIdentical(left->right, right->right);
}


int main(int argc, char *argv[])
{
    struct TreeNode *root  = NULL;
    struct TreeNode *other = NULL;
    int              identical;


    root = addTreeNode(root, 5);
    root = addTreeNode(root, 3);
    root = addTreeNode(root, 7);
    root = addTreeNode(root, 4);

    other = addTreeNode(other, 5);
    other = addTreeNode(other, 3);
    other = addTreeNode(other, 7);
    other = addTreeNode(other, 4);


    printf("1st tree topology:\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");

    printf("2nd tree topology:\n");
    printTreeNodeInTreeTopology(other);
    printf("\n");

    identical = isTreeIdentical(root, other);
    if ( identical )
        printf("Both trees are identical\n");
    else
        printf("Both trees are not identical\n");


    // I do not care about memory release as it is a test, but we should have freeTree
    other = addTreeNode(NULL, 5);
    other = addTreeNode(other, 3);
    other = addTreeNode(other, 7);
    other = addTreeNode(other, 4);
    other = addTreeNode(other, 9);

    printf("\n");
    printf("2nd tree is changed, the tree topology:\n");
    printTreeNodeInTreeTopology(other);
    printf("\n");

    identical = isTreeIdentical(root, other);
    if ( identical )
        printf("Both trees are identical\n");
    else
        printf("Both trees are not identical\n");

    printf("\n");

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.


    return 0;
}
