/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * ADT Binary tree (with AVL self-balancing functionality)
 */

#ifndef AVLBSTREE_H_HAS_INCLUDED

#define AVLBSTREE_H_HAS_INCLUDED

#include "btree.h"

struct TreeNode * treeRebalance(struct TreeNode *root);
struct TreeNode * addTreeNodeAndRebalanceTree(struct TreeNode *root, int val);
struct TreeNode * delTreeNodeAndRebalanceTree(struct TreeNode *root, int val);
int               isTreeNodeBalanced(struct TreeNode *root);

#endif
