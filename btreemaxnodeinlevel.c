/* Copyright © 2021-2022 Chee Bin HOH. All rights reserved.
 *
 * Find the maximum number of nodes in a level of a tree.
 */

#include "btree.h"
#include "btreetraverse.h"
#include <stdio.h>
#include <stdlib.h>

void traverseDepthFirstAndCountNumberOfNode(struct TreeNode *root,
                                            int levelCnt[], int level) {
  if (NULL == root)
    return;

  levelCnt[level] = levelCnt[level] + 1;
  level++;

  traverseDepthFirstAndCountNumberOfNode(root->left, levelCnt, level);
  traverseDepthFirstAndCountNumberOfNode(root->right, levelCnt, level);
}

int findMaxNumberOfNodeInLevel(struct TreeNode *root) {
  int i;
  int max;
  int levelCnt[100]; // having a queue will help avoid this limit, TODO

  for (i = 0; i < sizeof(levelCnt) / sizeof(levelCnt[0]); i++)
    levelCnt[i] = 0;

  traverseDepthFirstAndCountNumberOfNode(root, levelCnt, 0);

  max = 0;
  for (i = 0; i < sizeof(levelCnt) / sizeof(levelCnt[0]); i++)
    if (levelCnt[i] > max)
      max = levelCnt[i];

  return max;
}

/*
 *               0
 *               |
 *      +--------+---------+
 *      1                  2
 *      |                  |
 * +----+----+       //----+----+
 * 3         4                  5
 *                              |
 *                    +---------+---------+
 *                    6                   8
 *                    |                   |
 *              //----+----+          //--+--//
 *                         7
 */
int main(int argc, char *argv[]) {
  struct TreeNode *root = NULL;
  struct TreeNode *other = NULL;
  struct TreeNode *extendedBranch;

  root = malloc(sizeof(struct TreeNode));
  root->val = 0;
  root->left = NULL;
  root->right = NULL;

  other = malloc(sizeof(struct TreeNode));
  other->val = 1;
  other->left = NULL;
  other->right = NULL;
  root->left = other;

  other = malloc(sizeof(struct TreeNode));
  other->val = 3;
  other->left = NULL;
  other->right = NULL;
  root->left->left = other;
  extendedBranch = other;

  other = malloc(sizeof(struct TreeNode));
  other->val = 4;
  other->left = NULL;
  other->right = NULL;
  root->left->right = other;

  other = malloc(sizeof(struct TreeNode));
  other->val = 2;
  other->left = NULL;
  other->right = NULL;
  root->right = other;

  other = malloc(sizeof(struct TreeNode));
  other->val = 5;
  other->left = NULL;
  other->right = NULL;
  root->right->right = other;

  other = malloc(sizeof(struct TreeNode));
  other->val = 6;
  other->left = NULL;
  other->right = NULL;
  root->right->right->left = other;

  other = malloc(sizeof(struct TreeNode));
  other->val = 7;
  other->left = NULL;
  other->right = NULL;
  root->right->right->left->right = other;

  other = malloc(sizeof(struct TreeNode));
  other->val = 8;
  other->left = NULL;
  other->right = NULL;
  root->right->right->right = other;

  printf("The tree topology:\n");
  printf("\n");
  printTreeNodeInTreeTopology(root);

  printf("\n");
  printf("The maximum number of nodes in a level is %d\n",
         findMaxNumberOfNodeInLevel(root));
  printf("\n");

  other = malloc(sizeof(struct TreeNode));
  other->val = 30;
  other->left = NULL;
  other->right = NULL;

  extendedBranch->left = other;

  other = malloc(sizeof(struct TreeNode));
  other->val = 300;
  other->left = NULL;
  other->right = NULL;

  extendedBranch->right = other;

  printf("\n");
  printf("Extending branch 3 with two child nodes, the new tree topology:\n");
  printf("\n");
  printTreeNodeInTreeTopology(root);

  printf("\n");
  printf("The maximum number of nodes in a level is %d\n",
         findMaxNumberOfNodeInLevel(root));
  printf("\n");
  printf("\n");

  // I do not care about freeing malloced memory, OS will take care of freeing
  // heap that is part of process for this one off program.

  return 0;
}
