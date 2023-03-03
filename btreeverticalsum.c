/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * Find the vertical sum of a binary tree. Assume that the left and right child
 * of a node makes 45 degree angle with the parent.
 */

#include "btree.h"
#include "llist.h"
#include <stdio.h>
#include <stdlib.h>

struct VerticalSum {
  struct ListNode *llist;
  struct ListNode *rlist;
};

void traverseVerticalSumCallback(struct TreeNode *node, int pos, int axis,
                                 int *stop, void *data) {
  struct VerticalSum *pData;
  struct ListNode *lnode;

  pData = data;
  if (axis >= 0) {
    lnode = findNthListNode(pData->rlist, axis);
    if (NULL == lnode)
      enQueueInt(node->val, &((pData)->rlist));
    else
      lnode->data.val += node->val;
  } else {
    lnode = findNthListNode(pData->llist, (axis * -1) - 1);
    if (NULL == lnode)
      enQueueInt(node->val, &((pData)->llist));
    else
      lnode->data.val += node->val;
  }
}

struct ListNode *calculateBTreeVerticalSum(struct TreeNode *root) {
  struct VerticalSum data;
  struct ListNode *list;
  struct ListNode *node;

  data.llist = NULL;
  data.rlist = NULL;

  traverseTreeNodeInVerticalOrderTopDown(root, traverseVerticalSumCallback,
                                         &data);

  list = data.llist;
  if (NULL == list)
    list = data.rlist;
  else {
    list = reverseQueue(list);
    node = list;

    while (NULL != node->next)
      node = node->next;

    node->next = data.rlist;
  }

  return list;
}

/*
 */
int main(int argc, char *argv[]) {
  struct TreeNode *root;
  struct TreeNode *other;
  struct ListNode *resultlist;
  struct ListNode *listIter;

  root = NULL;
  root = malloc(sizeof(struct TreeNode));
  root->val = 1;
  root->left = NULL;
  root->right = NULL;

  other = malloc(sizeof(struct TreeNode));
  other->val = 2;
  other->left = NULL;
  other->right = NULL;
  root->left = other;

  other = malloc(sizeof(struct TreeNode));
  other->val = 3;
  other->left = NULL;
  other->right = NULL;
  root->right = other;

  other = malloc(sizeof(struct TreeNode));
  other->val = 6;
  other->left = NULL;
  other->right = NULL;
  root->right->right = other;

  other = malloc(sizeof(struct TreeNode));
  other->val = 5;
  other->left = NULL;
  other->right = NULL;
  root->right->left = other;

  other = malloc(sizeof(struct TreeNode));
  other->val = 7;
  other->left = NULL;
  other->right = NULL;
  root->right->left->left = other;

  other = malloc(sizeof(struct TreeNode));
  other->val = 8;
  other->left = NULL;
  other->right = NULL;
  root->right->left->right = other;

  printf("The tree topology:\n");
  printf("\n");
  printTreeNodeInTreeTopology(root);
  printf("\n");

  printf("\n");
  printf("The tree in aligned vertical axis\n");
  printf("\n");
  printf("\t\t1\n");
  printf("\n");
  printf("\t2\t\t3\n");
  printf("\n");
  printf("\t\t5\t\t6\n");
  printf("\n");
  printf("\t7\t\t8\n");
  printf("\n");

  printf("\n");
  printf("Vertical node sum:\n");
  printf("\n");
  resultlist = calculateBTreeVerticalSum(root);
  listIter = resultlist;
  while (NULL != listIter) {
    printf("\t%d", listIter->data.val);
    listIter = listIter->next;
  }

  freeQueue(&resultlist);

  printf("\n");
  printf("\n");
  printf("\n");

  // I do not care about freeing malloced memory, OS will take care of freeing
  // heap that is part of process for this one off program.

  return 0;
}
