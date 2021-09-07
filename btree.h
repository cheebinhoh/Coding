/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * ADT Binary tree (with AVL self-balancing functionality)
 */

#ifndef BTREE_H_HAS_INCLUDED

#define BTREE_H_HAS_INCLUDED


/* Definition for a binary tree node.
 */
struct TreeNode {

     int              val;
     struct TreeNode *left;
     struct TreeNode *right;
};

typedef void (bTreeTraversalCallback)(struct TreeNode *node, int pos, void *data);

struct TreeNode * addTreeNode(struct TreeNode *root, int val);
struct TreeNode * findTreeNode(struct TreeNode *root, int val);
struct TreeNode * mirrorSwap(struct TreeNode *root);
struct TreeNode * treeRebalance(struct TreeNode *root);
struct TreeNode * addTreeNodeAndRebalanceTree(struct TreeNode *root, int val);
void traverseTreeNodeInOrder(struct TreeNode *root, bTreeTraversalCallback func, void *data);
int isTreeIdentical(struct TreeNode *left, struct TreeNode *right);
int findTotalNumberOfTreeNode(struct TreeNode *root);
void printTreeNodeInOrder(struct TreeNode *root);
void printTreeNodeInTreeTopology(struct TreeNode *root);


#endif
