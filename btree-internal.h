/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * ADT Binary tree (with AVL self-balancing functionality)
 */

#ifndef BTREE_INTERNAL_H_HAS_INCLUDED

#define BTREE_INTERNAL_H_HAS_INCLUDED

/* Definition for a binary tree node.
 */
struct TreeNode {

     int              val;
     struct TreeNode *left;
     struct TreeNode *right;
};

int determineMaxDepthLevelRecursive(struct TreeNode *root, int level);

#endif
