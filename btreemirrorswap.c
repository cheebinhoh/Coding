/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Given the root node of a binary tree, swap the ‘left’ and ‘right’ children for each node.
 */

#include <stdio.h>
#include "btree.h"


int main(int argc, char *argv[])
{
    struct TreeNode *root = NULL;


    root = addTreeNode(root, 5);
    root = addTreeNode(root, 3);
    root = addTreeNode(root, 7);
    root = addTreeNode(root, 4);

    printf("The tree topology:\n");
    printf("\n");
    printTreeNodeInTreeTopology(root);
    printf("\n"); // we can introduce wrapper entry call to printTreeNodeInOrder to printf \n
    printf("\n");

    treeMirrorSwap(root);

    printf("The tree topology after mirror swap of tree node:\n");
    printf("\n");
    printTreeNodeInTreeTopology(root);
    printf("\n"); // we can introduce wrapper entry call to printTreeNodeInOrder to printf \n
    printf("\n");

    return 0;
}
