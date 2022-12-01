/* Copyright © 2021-2022 Chee Bin HOH. All rights reserved.
 *
 * Determine if a tree is AVL balanced.
 */

#include "btree.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  struct TreeNode *root = NULL;

  printf("Binary tree of adding 3, 2, 4, 5, 6, the tree topology:\n");
  root = addTreeNode(root, 3);
  root = addTreeNode(root, 2);
  root = addTreeNode(root, 4);
  root = addTreeNode(root, 5);
  root = addTreeNode(root, 6);

  printf("\n");
  printTreeNodeInTreeTopology(root);
  printf("\n");
  printf("\n");

  if (!isTreeNodeBalanced(root)) {
    printf("The tree is not AVL balanced, rebalancing the tree, the tree "
           "topology:\n");
    printf("\n");

    root = treeRebalance(root);
    printTreeNodeInTreeTopology(root);
    printf("\n");

    if (isTreeNodeBalanced(root))
      printf("The tree is now AVL balanced\n");

    printf("\n");
  }

  printf("\n");

  return 0;
}
