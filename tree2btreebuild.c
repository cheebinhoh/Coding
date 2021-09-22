/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Build binary tree from an none binary tree which # of childs can be more than 2.
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"
#include "btreetraverse.h"


struct NTreeNode {

    int               val;
    int               numberOfChild;
    struct NTreeNode *child[50];     // let give it an artificial limit
};


void printNTreeInMultiLevel(struct NTreeNode *list[],
                            int               listCnt,
                            int               level)
{
    int i;
    int j;


    if ( 0 == listCnt )
        return;

    for ( i = listCnt - 1; i >= 0; i-- )
    {
        for ( j = 0; j < level * 5; j++ )
            putchar(' ');

        printf("%d\n", (list[i])->val);
        printNTreeInMultiLevel((list[i])->child, (list[i])->numberOfChild, level + 1);
    }
}


void printNTree(struct NTreeNode *root)
{
    printNTreeInMultiLevel(&root, 1, 0);
}


struct TreeNode * ntree2btreeInternal(struct TreeNode  *broot,
                                      struct NTreeNode *list[],
                                      int               listCnt)
{
    int              i;
    struct TreeNode *node = NULL;
    struct TreeNode *prev = NULL;


    for ( i = 0; i < listCnt; i++ )
    {
        node      = malloc(sizeof( struct TreeNode ));
        node->val = list[i]->val;

        if ( NULL == broot )
        {
            broot = node;
        }
        else
        {
            if ( NULL == prev )
            {
                broot->left = node;
            }
            else
            {
                prev->right = node;
            }

            prev = node;
        }

        ntree2btreeInternal(node, list[i]->child, list[i]->numberOfChild);
    }

    return broot;
}


struct TreeNode * ntree2btree(struct NTreeNode *root)
{
    struct TreeNode *broot = NULL;


    return ntree2btreeInternal(broot, &root, 1);
}

struct TreeNode * ntree2bstreeInternal(struct NTreeNode *root, struct TreeNode *bsroot)
{
    int i;


    if ( NULL == root )
        return bsroot;

    bsroot = addTreeNodeAndRebalanceTree(bsroot, root->val);
    for ( i = 0; i < root->numberOfChild; i++ )
        bsroot = ntree2bstreeInternal(root->child[i], bsroot);

    return bsroot;
}


struct TreeNode * ntree2bstree(struct NTreeNode *root)
{
    struct TreeNode *bsroot = NULL;


    return ntree2bstreeInternal(root, bsroot);
}


/* The logic to convert a generic sorted tree into a binary tree is that:
 *
 * - right sibling of the current node is added as right branch node of the current node.
 * - child of the current node is added as left branch nodes of the current node.
 * - repeat above two steps for each node recursively.
 *
 *
 *                  1
 *                  |
 *         +--------+--------+
 *         10       20       30
 *         |
 *  +------+-----+
 *  100    101   102
 *
 *
 *  binary tree:
 *
 *                 1
 *                 |
 *           +-----------+
 *           10          //
 *           |
 *     +-----------------+
 *     100               20
 *     |                  |
 *     -----+             +-----+
 *          101                 30
 *          |
 *          -----+
 *               102
 *
 * in order = 100, 101, 102, 10, 20, 30, 1
 */
int main(int argc, char * argv[])
{
    struct NTreeNode *root;
    struct NTreeNode *other;
    struct  TreeNode *broot;
    int               n;


    root = malloc(sizeof( struct NTreeNode ));
    root->val = 1;
    root->numberOfChild = 0;

    n = 0;
    other = malloc(sizeof( struct NTreeNode ));
    other->val           = 10;
    other->numberOfChild = 0;
    root->child[n++]     = other;

    other = malloc(sizeof( struct NTreeNode ));
    other->val           = 20;
    other->numberOfChild = 0;
    root->child[n++]     = other;

    other = malloc(sizeof( struct NTreeNode ));
    other->val           = 30;
    other->numberOfChild = 0;

    root->child[n++]     = other;
    root->numberOfChild  = n;

    n = 0;

    other = malloc(sizeof( struct NTreeNode ));
    other->val                 = 100;
    other->numberOfChild       = 0;
    root->child[0]->child[n++] = other;

    other = malloc(sizeof( struct NTreeNode ));
    other->val                 = 101;
    other->numberOfChild       = 0;
    root->child[0]->child[n++] = other;

    other = malloc(sizeof( struct NTreeNode ));
    other->val                    = 102;
    other->numberOfChild          = 0;
    root->child[0]->child[n++]    = other;
    root->child[0]->numberOfChild = n;

    printf("Nary tree topology:\n");
    printf("\n");
    printNTree(root);
    printf("\n");

    printf("\n");
    broot = ntree2bstree(root);
    printf("Build a binary search tree, the tree topology:\n");
    printf("\n");

    printTreeNodeInTreeTopology(broot);
    printf("\n");

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    return 0;
 }
