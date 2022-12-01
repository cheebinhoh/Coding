/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Depth first search, preorder, inorder and postorder in iterative way.
 */

#include "btreetraverse.h"
#include "btree.h"
#include <stdio.h>
#include <stdlib.h>

/* A post order traverse in iterative way:
 *
 * If we are at a node, we will process left child node (if any), and then
 * process right child node (if any), and then the node that owns the left and
 * right child nodes. When we are in a child node, we will apply same processing
 * ordering (left, right and node itself).
 *
 * It is a depth first approach starting at the left branch, so we will further
 * deeper on the left and then to right branch, and apply the same left, right
 * and node ordering, then go up one level to another right branch and apply the
 * same left, right and node ordering.
 *
 * How to do it in none-recursive fashion?
 *
 * - we do none-recursive loop to traverse the tree.
 * - we start at the root, if root is not NULL, we continue the loop.
 * - if root->left is not NULL, we need to process left branch and then
 * backtrack to right branch and then parent node of the branches, so we store
 * root (parent node) in rightPendingList (the name indicates that right branch
 * is pending to be processed), and then assign root->left to be new root and
 * continue the loop.
 * - if root->left is NULL, but root->right is not NULL, then we need to process
 * the right node and then back to the root later, let store the root in
 * topPendingList so we want to backtrack to it after we exhaust the right
 * branch, we assign root->right to be new root and continue the loop.
 * - if we have exhausted left or right branches of a node (root), then we need
 * to bracktrack, so the backtracking is logic as that
 *   -- if topPendingList is not empty, and we peek at the top (of
 * topPendingList), and if the top's right branch is equal to the root now, then
 * we are at the right branch of top, then we pop out top from topPendingList,
 * and assigning it to the root, and continue the loop, eventually we are
 * backtracking from right branch backward to higher branch of the tree.
 *
 *      we will continue that backtracking from right branch up the tree until
 * we are reach a top from topPendingList that it right branch is not equal to
 * root.
 *
 *   -- once we backtrack enough from right branch either by exhausting the
 * topPendingList, or reach top of the topPendingList that its right branch is
 * not equal to root, then we will process rightPendingList if its node has
 * right branch.
 *
 * rightPendingList is to store node that its right branch is pending to be
 * processed. topPendingList is to store node that its left branch has
 * processed, right branch is now in process, but its node is not yet processed.
 */
void postOrderTraversal(struct TreeNode *root) {
  struct TreeNode *rightPendingList[100];
  struct TreeNode *topPendingList[100];
  int rightPendingIndex = 0;
  int topPendingIndex = 0;
  int count = 0; // a fail-safe to prevent infinite loop
  int start = 0;

  while (NULL != root && count < 1000) {
    if (NULL != root->left) {
      rightPendingList[rightPendingIndex++] = root;
      root = root->left;
    } else if (NULL != root->right) {
      topPendingList[topPendingIndex++] = root;
      root = root->right;
    } else {
      if (start)
        printf(", ");

      printf("%d", root->val);
      start = 1;

      while (topPendingIndex > 0 &&
             topPendingList[topPendingIndex - 1]->right == root) {
        root = topPendingList[--topPendingIndex];
        printf(", %d", root->val);
      }

      root = NULL;
      while (rightPendingIndex > 0 &&
             ((root = rightPendingList[--rightPendingIndex])->right) == NULL) {
        printf(", %d", root->val);
        root = NULL;
      }

      if (NULL != root) {
        topPendingList[topPendingIndex++] = root;
        root = root->right;
      }
    }

    count++;
  }

  if (count > 0)
    printf("\n");
}

/* An in order traversal logic:
 *
 * We process left branch of a node, the node and then right branch, we traverse
 * tree in depth first approach, so if there is left branch, we will proceed
 * further left as far as possible, then the node (in bottom) and then right
 * branch, when we are in right branch, we will repeat above.
 *
 * It is simple than postorder logic that we only need a topPendingList that
 * stores a list of node that we want to process after processing the left
 * branch.
 */
void inOrderTraversal(struct TreeNode *root) {
  struct TreeNode *topPendingList[100];
  int topPendingListIndex = 0;
  int count = 0;
  int start = 0;

  while (NULL != root && count < 1000) {
    count++;

    if (NULL != root->left) {
      topPendingList[topPendingListIndex++] = root;
      root = root->left;
    } else {
      if (start)
        printf(", ");

      printf("%d", root->val);
      start = 1;

      if (NULL != root->right) {
        root = root->right;
      } else if (topPendingListIndex > 0) {
        do {
          root = topPendingList[--topPendingListIndex];
          printf(", %d", root->val);

          root = root->right;
        } while (NULL == root && topPendingListIndex > 0);
      } else {
        root = NULL;
      }
    }
  }

  if (count > 0)
    printf("\n");
}

/* A pre order traversal logic:
 *
 * We process the node, the left branch, and then right branch, we traverse tree
 * in depth first approach, so if there is left branch, we will proceed further
 * left as far as possible, and then right branch, when we are in right branch,
 * in each next level, we will repeat above.
 *
 * It is simple than postorder logic that we only need a rightPendingList that
 * stores a list of right branch node that we want to process after processing
 * the left branch.
 */
void preOrderTraversal(struct TreeNode *root) {
  struct TreeNode *rightPendingList[100];
  int rightPendingListIndex = 0;
  int count = 0;

  while (NULL != root && count < 1000) {
    count++;

    if (count > 1)
      printf(", ");

    printf("%d", root->val);

    if (NULL != root->left) {
      if (NULL != root->right)
        rightPendingList[rightPendingListIndex++] = root->right;

      root = root->left;
    } else if (NULL != root->right) {
      root = root->right;
    } else {
      if (rightPendingListIndex > 0)
        root = rightPendingList[--rightPendingListIndex];
      else
        root = NULL;
    }
  }

  if (count > 0)
    printf("\n");
}
