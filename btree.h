/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * ADT binary search tree
 */

#ifndef BTREE_H_HAS_INCLUDED

#define BTREE_H_HAS_INCLUDED

#include "btree-internal.h"
#include "avlbstree.h"


typedef void (bTreeTraversalCallback)(struct TreeNode *node, int pos, void *data);


struct TreeNode * addTreeNode(struct TreeNode *root, int val);
struct TreeNode * findTreeNode(struct TreeNode *root, int val);
struct TreeNode * treeRebalance(struct TreeNode *root);
struct TreeNode * delTreeNode(struct TreeNode *root, int val);
void              freeTreeNode(struct TreeNode *root);

void printTreeNodeInOrder(struct TreeNode *root);
void printTreeNodeInTreeTopology(struct TreeNode *root);

int  findTotalNumberOfTreeNode(struct TreeNode *root);
void traverseTreeNodeInOrder(struct TreeNode *root, bTreeTraversalCallback func, void *data);

int determineMaxDepthLevel(struct TreeNode *root);
int determineMinDepthLevel(struct TreeNode *root);

int isTreeBinarySearchTree(struct TreeNode *root);

struct TreeNode * findLeastCommonAncestor(struct TreeNode *root, int val1, int val2);
int findMaximumSumPathToLeafValue(struct TreeNode *root, int val);
int isSubBinaryTree(struct TreeNode *tree1, struct TreeNode *tree2);

struct ListNode * getInOrderList(struct TreeNode *root);
struct ListNode * getPreOrderList(struct TreeNode *root);
struct ListNode * getPostOrderList(struct TreeNode *root);


#endif
