/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Given the roots of two binary trees, determine if these trees are
 * identical or not. Identical trees have the same layout and data at each node.
 */

#include <stdio.h>
#include "btree.h"


int main(int argc, char *argv[])
{
    struct TreeNode *root = NULL;


    printf("Binary tree of adding 3, 2, 4, 5, 6\n");
    root = addTreeNode(root, 3);
    root = addTreeNode(root, 2);
    root = addTreeNode(root, 4);
    root = addTreeNode(root, 5);
    root = addTreeNode(root, 6);

    printTreeNodeInTreeTopology(root);
    printf("\n");

    if ( ! isTreeNodeBalanced(root) )
    {
        printf("The tree is not AVL balanced, rebalancing the tree\n");

        root = treeRebalance(root);
        printTreeNodeInTreeTopology(root);
        printf("\n");

        if ( isTreeNodeBalanced(root) )
            printf("The tree is now AVL balanced\n");
    }

    return 0;
}
