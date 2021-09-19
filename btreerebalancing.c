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


    printf("Binary tree topology of adding 6, 7, 8, 5, 4, 10, 9\n");
    root = addTreeNode(root, 6);
    root = addTreeNode(root, 7);
    root = addTreeNode(root, 8);
    root = addTreeNode(root, 5);
    root = addTreeNode(root, 4);
    root = addTreeNode(root, 10);
    root = addTreeNode(root, 9);

    printTreeNodeInTreeTopology(root);
    printf("\n");

    if ( ! isTreeNodeBalanced(root) )
    {
        printf("The binary tree is not AVL balanced\n");
        printf("\n");
    }

    root = delTreeNode(root, 8);
    printf("After deleing 8, the tree is\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");

    if ( ! isTreeNodeBalanced(root) )
    {
        printf("The binary tree is not AVL balanced\n");
        printf("\n");
    }

    root = NULL; // I do not care about memory leak for this simple test program
    printf("Self balancing binary tree of adding 6, 7, 8, 5, 4, 10, 9, 11\n");
    root = addTreeNodeAndRebalanceTree(root, 6);
    root = addTreeNodeAndRebalanceTree(root, 7);
    root = addTreeNodeAndRebalanceTree(root, 8);
    root = addTreeNodeAndRebalanceTree(root, 5);
    root = addTreeNodeAndRebalanceTree(root, 4);
    root = addTreeNodeAndRebalanceTree(root, 10);
    root = addTreeNodeAndRebalanceTree(root, 9);
    root = addTreeNodeAndRebalanceTree(root, 11);

    printTreeNodeInTreeTopology(root);
    printf("\n");

    if ( isTreeNodeBalanced(root) )
    {
        printf("The binary tree is AVL balanced\n");
        printf("\n");
    }


    root = delTreeNodeAndRebalanceTree(root, 5);
    root = delTreeNodeAndRebalanceTree(root, 6);
    printf("After deleing 5 and 6, the tree topology:\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");

    if ( isTreeNodeBalanced(root) )
    {
        printf("The binary tree is AVL balanced\n");
        printf("\n");
    }

    return 0;
}
