/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Directed graph
 */

#include <stdlib.h>
#include "dgraph.h"

struct DGraphNode * addGraphNodeInt(struct DGraph **graph, struct DGraphNode *from, int val)
{
    struct DGraphNode *newNode;
    struct DGraph     *newGraph;


    newNode = NULL;

    if ( NULL == *graph )
    {
        newGraph = malloc(sizeof(struct DGraph));

        newGraph->vertices = NULL;
        newGraph->root     = NULL;
        *graph             = newGraph;
    }

    newNode = malloc(sizeof(struct DGraphNode));
    if ( NULL == newNode )
        goto releaseGraph;

    newNode->data.val = val;
    newNode->edges    = NULL;

    if ( NULL == from )
    {
        if ( NULL != (*graph)->root )
            enQueueRef((*graph)->root, &(newNode->edges));

        (*graph)->root = newNode;
    }
    else
    {
        enQueueRef(newNode, &(from->edges));
    }

    enQueueRef(newNode, &((*graph)->vertices));

    return newNode;

releaseGraph:
    if ( NULL != newGraph )
    {
        *graph = NULL;
        free(newGraph);
    }

    return NULL;
}


int traverseDGraphRecursively(struct DGraphNode        *parent,
                              struct DGraphNode        *node,
                              struct ListNode         **visited,
                              DGraphTraversalCallback   func,
                              void                     *data)
{
    int              stop = 0;
    struct ListNode *iter = NULL;


    if ( NULL == node )
        return stop;

    iter = *visited;
    while ( NULL != iter
            && node != iter->data.ref )
        iter = iter->next;

    if ( NULL != iter )
        return stop;

    func(parent, node, &stop, data);
    if ( stop )
        return stop;

    enQueueRef(node, visited);

    parent = node;
    iter   = node->edges;
    while ( NULL != iter )
    {
        node = iter->data.ref;

        stop = traverseDGraphRecursively(parent, node, visited, func, data);
        if ( stop )
            return stop;

        iter = iter->next;
    }

    return 0;
}

void traverseDGraph(struct DGraph *graph, DGraphTraversalCallback func, void *data)
{
    struct ListNode *visited;


    visited = NULL;
    traverseDGraphRecursively(NULL, graph->root, &visited, func, data);
}


void linkDGraphNode(struct DGraphNode *from, struct DGraphNode *node)
{
    struct ListNode *iter;


    iter = from->edges;
    while ( NULL != iter
            && node != iter->data.ref )
        iter = iter->next;

    if ( NULL != iter )
        return;

    enQueueRef(node, &(from->edges));
}
