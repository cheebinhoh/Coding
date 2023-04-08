/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * ADT Binary tree (with AVL self-balancing functionality).
 */

#ifndef BTREE_INTERNAL_H_HAS_INCLUDED

#define BTREE_INTERNAL_H_HAS_INCLUDED

struct TreeNode {
  int val;
  struct TreeNode *left;
  struct TreeNode *right;
};

#endif
