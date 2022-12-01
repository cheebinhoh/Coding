/* Copyright © 2021-2022 Chee Bin HOH. All rights reserved.
 *
 * Directed graph
 */

#include "dgraph.h"
#include <stdio.h>

void traverseDGraphNode(struct DGraphNode *from, struct DGraphNode *node,
                        int *stop, void *data) {
  if (NULL != from)
    printf("%d -> ", from->data.val);

  printf("%d\n", node->data.val);
}

int main(int argc, char *argv[]) {
  struct DGraph *graph;
  struct DGraph *cloneGraph;
  struct DGraphNode *parent;
  struct DGraphNode *node1;
  struct DGraphNode *node2;
  struct DGraphNode *node3;
  struct DGraphNode *node4;

  graph = NULL;
  parent = addGraphNodeInt(&graph, NULL, 0);
  node1 = addGraphNodeInt(&graph, parent, 1);
  node2 = addGraphNodeInt(&graph, parent, 2);
  linkDGraphNode(node1, node2);

  node3 = addGraphNodeInt(&graph, node1, 3);
  linkDGraphNode(node2, node3);

  node4 = addGraphNodeInt(&graph, node1, 4);
  linkDGraphNode(node3, node4);

  printf("Traverse graph:\n");
  traverseDGraph(graph, traverseDGraphNode, NULL);
  printf("\n");

  printf("Traverse graph uniquely:\n");
  traverseDGraphUniquely(graph, traverseDGraphNode, NULL);
  printf("\n");

  cloneGraph = cloneGraphInt(graph);
  printf("Traverse cloned graph:\n");
  traverseDGraph(cloneGraph, traverseDGraphNode, NULL);
  printf("\n");

  printf("Traverse clone graph uniquely:\n");
  traverseDGraphUniquely(cloneGraph, traverseDGraphNode, NULL);
  printf("\n");

  return 0;
}
