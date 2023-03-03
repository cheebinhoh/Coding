/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * Find the maximum path sum. The path may start and end at any node in the
 * tree.
 */

#include "btree.h"
#include <stdio.h>
#include <stdlib.h>

int btreePathSumInternal(struct TreeNode *root, int sum) {
  int lsum;
  int rsum;

  if (NULL == root)
    return sum;

  sum = sum + root->val;
  lsum = sum;
  rsum = sum;

  if (NULL != root->left)
    lsum = btreePathSumInternal(root->left, sum);

  if (NULL != root->right)
    rsum = btreePathSumInternal(root->right, sum);

  if (lsum > sum)
    sum = lsum;

  if (rsum > sum)
    sum = rsum;

  return sum;
}

/* This is API facing method to check if there is a path of sum of value of
 * nodes that matches the target value.
 */
int btreeMaximumPathSum(struct TreeNode *root) {
  int sum = 0;

  return btreePathSumInternal(root, sum);
}

int main(int argc, char *argv[]) {
  struct TreeNode *root = NULL;
  struct TreeNode *other = NULL;

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
  printTreeNodeInTreeTopology(root);
  printf("\n");

  printf("The maximum path sum is %d\n", btreeMaximumPathSum(root));
  printf("\n");
  printf("\n");

  // I do not care about freeing malloced memory, OS will take care of freeing
  // heap that is part of process for this one off program.

  return 0;
}
