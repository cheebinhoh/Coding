/* Copyright © 2021-2022 Chee Bin HOH. All rights reserved.
 *
 * Directed graph
 */

#ifndef DGRAPH_INTERNAL_H_HAS_INCLUDED

#define DGRAPH_INTERNAL_H_HAS_INCLUDED

#include "llist.h"

union DGraphNodeData {
  void *ref;
  int val;
};

struct DGraphNode {
  union DGraphNodeData data;
  struct ListNode *edges;
};

struct DGraph {
  struct DGraphNode *root;
  struct ListNode *vertices;
};

#endif
