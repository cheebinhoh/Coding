/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Given the root node of a binary tree, swap the ‘left’ and ‘right’ children for each node.
 */

#include <stdio.h>
#include "btree.h"


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

int main(int argc, char *argv[])
{
    struct TreeNode *root = NULL;


    root = addTreeNode(root, 5);
    root = addTreeNode(root, 3);
    root = addTreeNode(root, 7);
    root = addTreeNode(root, 4);

    printf("Ascending order = ");
    printTreeNodeInOrder(root);
    printf("\n"); // we can introduce wrapper entry call to printTreeNodeInOrder to printf \n

    printf("\n");
    printf("Executing mirror swap of tree\n");
    root = mirrorSwap(root);
    printf("Descending order = ");
    printTreeNodeInOrder(root);
    printf("\n"); // we can introduce wrapper entry call to printTreeNodeInOrder to printf \n

    return 0;
}
