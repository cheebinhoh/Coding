/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * Directed graph
 */

#include "dgraph.h"
#include <stdlib.h>

struct DGraphNode *addGraphNodeInt(struct DGraph **graph,
                                   struct DGraphNode *from, int val) {
  struct DGraphNode *newNode;
  struct DGraph *newGraph;

  newNode = NULL;

  if (NULL == *graph) {
    newGraph = malloc(sizeof(struct DGraph));

    newGraph->vertices = NULL;
    newGraph->root = NULL;
    *graph = newGraph;
  }

  newNode = malloc(sizeof(struct DGraphNode));
  if (NULL == newNode)
    goto releaseGraph;

  newNode->data.val = val;
  newNode->edges = NULL;

  if (NULL == from) {
    if (NULL != (*graph)->root)
      enQueueRef((*graph)->root, &(newNode->edges));

    (*graph)->root = newNode;
  } else {
    enQueueRef(newNode, &(from->edges));
  }

  enQueueRef(newNode, &((*graph)->vertices));

  return newNode;

releaseGraph:
  if (NULL != newGraph) {
    *graph = NULL;
    free(newGraph);
  }

  return NULL;
}

int traverseDGraphRecursively(struct DGraphNode *parent,
                              struct DGraphNode *node,
                              struct ListNode **visited,
                              DGraphTraversalCallback func, void *data) {
  int stop = 0;
  struct ListNode *iter = NULL;

  if (NULL == node)
    return stop;

  if (NULL != visited) {
    iter = *visited;
    while (NULL != iter && node != iter->data.ref)
      iter = iter->next;

    if (NULL != iter)
      return stop;

    enQueueRef(node, visited);
  }

  func(parent, node, &stop, data);
  if (stop)
    return stop;

  parent = node;
  iter = node->edges;
  while (NULL != iter) {
    node = iter->data.ref;

    stop = traverseDGraphRecursively(parent, node, visited, func, data);
    if (stop)
      return stop;

    iter = iter->next;
  }

  return 0;
}

struct DGraph *cloneGraphInt(struct DGraph *graph) {
  struct DGraph *newGraph;
  struct DGraphNode *newNode;
  struct DGraphNode *node;
  struct ListNode *edge;
  struct DGraphNode *otherNode;
  struct DGraphNode *otherNodeNew;
  struct ListNode *iter;
  struct ListNode *iterNew;
  struct ListNode *listOther;
  int n;

  newGraph = malloc(sizeof(struct DGraph));
  if (NULL == newGraph)
    return NULL;

  newGraph->vertices = NULL;
  newGraph->root = NULL;

  iter = graph->vertices;
  while (NULL != iter) {
    newNode = malloc(sizeof(struct DGraphNode));
    if (NULL == newNode)
      goto failure;

    node = iter->data.ref;

    newNode->data.val = node->data.val;
    newNode->edges = NULL;

    enQueueRef(newNode, &(newGraph->vertices));

    if (node == graph->root)
      newGraph->root = newNode;

    iter = iter->next;
  }

  iter = graph->vertices;
  iterNew = newGraph->vertices;
  while (NULL != iter) {
    newNode = iterNew->data.ref;
    node = iter->data.ref;

    edge = node->edges;
    while (NULL != edge) {
      otherNode = edge->data.ref;
      n = findListNodeRefIndex(graph->vertices, otherNode);

      listOther = findNthListNode(newGraph->vertices, n);

      enQueueRef(listOther->data.ref, &(newNode->edges));

      edge = edge->next;
    }

    iter = iter->next;
    iterNew = iterNew->next;
  }

  return newGraph;

failure:
  freeGraph(newGraph);

  return NULL;
}

void freeGraph(struct DGraph *graph) {
  struct ListNode *iter;
  struct ListNode *tmp;

  if (NULL == graph)
    return;

  iter = graph->vertices;
  while (NULL != iter) {
    tmp = iter->next;
    free(iter->data.ref);
    free(iter);

    iter = tmp;
  }

  free(graph);
}

void traverseDGraph(struct DGraph *graph, DGraphTraversalCallback func,
                    void *data) {
  traverseDGraphRecursively(NULL, graph->root, NULL, func, data);
}

void traverseDGraphUniquely(struct DGraph *graph, DGraphTraversalCallback func,
                            void *data) {
  struct ListNode *visited;

  visited = NULL;
  traverseDGraphRecursively(NULL, graph->root, &visited, func, data);

  if (NULL != visited)
    freeList(&visited);
}

void linkDGraphNode(struct DGraphNode *from, struct DGraphNode *node) {
  struct ListNode *iter;

  iter = from->edges;
  while (NULL != iter && node != iter->data.ref)
    iter = iter->next;

  if (NULL != iter)
    return;

  enQueueRef(node, &(from->edges));
}
