/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Directed graph
 */


#ifndef DGRAPH_H_HAS_INCLUDED

#define DGRAPH_H_HAS_INCLUDED

#include "dgraph-internal.h"

typedef void (DGraphTraversalCallback)(struct DGraphNode *from, struct DGraphNode *, int *stop, void *data);

struct DGraphNode * addGraphNodeInt(struct DGraph **graph, struct DGraphNode *from, int val);
void                linkDGraphNode(struct DGraphNode *from, struct DGraphNode *node);

void traverseDGraph(struct DGraph *graph, DGraphTraversalCallback func, void *data);

#endif
