/* Copyright © 2021-2022 Chee Bin HOH. All rights reserved.
 *
 * AVL self-balancing tree
 */

#include "btree.h"
#include <stdio.h>

void checkIfTreeIsAVLRebalanced(struct TreeNode *root) {
  if (isTreeNodeBalanced(root)) {
    printf("The binary tree is AVL balanced\n");
  } else {
    printf("The binary tree is not AVL balanced\n");
  }
}

int main(int argc, char *argv[]) {
  struct TreeNode *root = NULL;

  printf("Binary tree topology of adding 6, 7, 8, 5, 4, 10, 9:\n");
  printf("\n");
  root = addTreeNode(root, 6);
  root = addTreeNode(root, 7);
  root = addTreeNode(root, 8);
  root = addTreeNode(root, 5);
  root = addTreeNode(root, 4);
  root = addTreeNode(root, 10);
  root = addTreeNode(root, 9);

  printTreeNodeInTreeTopology(root);
  printf("\n");

  checkIfTreeIsAVLRebalanced(root);
  printf("\n");
  printf("\n");

  root = delTreeNode(root, 8);
  printf("After deleting 8, the tree topology:\n");
  printf("\n");
  printTreeNodeInTreeTopology(root);
  printf("\n");

  checkIfTreeIsAVLRebalanced(root);
  printf("\n");
  printf("\n");

  root = NULL; // I do not care about memory leak for this simple test program
  printf("Self balancing binary tree of adding 6, 7, 8, 5, 4, 10, 9, 11:\n");
  printf("\n");

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

  checkIfTreeIsAVLRebalanced(root);
  printf("\n");
  printf("\n");

  root = delTreeNodeAndRebalanceTree(root, 5);
  root = delTreeNodeAndRebalanceTree(root, 6);
  printf("After deleting 5 and 6, the tree topology:\n");
  printf("\n");
  printTreeNodeInTreeTopology(root);
  printf("\n");

  checkIfTreeIsAVLRebalanced(root);
  printf("\n");
  printf("\n");

  return 0;
}
