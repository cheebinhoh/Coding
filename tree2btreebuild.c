/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Build binary tree from inorder and postorder list.
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


void printNTreeInSingleLevel(struct NTreeNode **list,
                             int                listCnt,
                             int                level)
{
    int i;


    printf("level %d = ", level);

    for ( i = 0; i < listCnt; i++ )
    {
        printf("%d, ", list[i]->val);
    }
}


void printNTreeInMultiLevel(struct NTreeNode *list[],
                            int               listCnt,
                            int               level)
{
    int                i;
    int                j;
    int                nextLevelListCnt  = 0;
    int                nextLevelListSize = 0;
    struct NTreeNode **nextLevelList     = NULL;


    printNTreeInSingleLevel(list, listCnt, level);
    printf("\n");

    for ( i = 0; i < listCnt; i++ )
    {
        for ( j = 0; j < list[i]->numberOfChild; j++ )
        {
            if ( nextLevelListCnt >= nextLevelListSize )
            {
                // WARNING: we do not check malloc and realloc return, in production code, we need!
                // it is also an inefficient allocation, we should have allocated N slot where N is increased in related
                // to number of child.

                if ( NULL == nextLevelList )
                {
                    nextLevelList = malloc(sizeof( struct NTreeNode * ) * 1);
                    nextLevelListSize += 1;
                }
                else
                {
                    nextLevelList = realloc(nextLevelList, sizeof( struct NTreeNode * ) * ( nextLevelListSize + 1 ) );
                    nextLevelListSize += 1;
                }
            }

            nextLevelList[nextLevelListCnt++] = list[i]->child[j];
        }
    }

    if ( nextLevelListCnt > 0 )
    {
        printNTreeInMultiLevel(nextLevelList, nextLevelListCnt, level + 1);
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
        node      = malloc(sizeof( struct TreeNode ) );
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
    int               n;
    struct  TreeNode *broot;


    root = malloc(sizeof( struct NTreeNode ));
    root->val = 1;
    root->numberOfChild = 0;

    n = 0;
    other = malloc(sizeof( struct NTreeNode ));
    other->val = 10;
    other->numberOfChild = 0;
    root->child[n++] = other;

    other = malloc(sizeof( struct NTreeNode ));
    other->val = 20;
    other->numberOfChild = 0;
    root->child[n++] = other;

    other = malloc(sizeof( struct NTreeNode ));
    other->val = 30;
    other->numberOfChild = 0;
    root->child[n++] = other;

    root->numberOfChild = n;

    n = 0;
    other = malloc(sizeof( struct NTreeNode ));
    other->val = 100;
    other->numberOfChild = 0;
    root->child[0]->child[n++] = other;

    other = malloc(sizeof( struct NTreeNode ));
    other->val = 101;
    other->numberOfChild = 0;
    root->child[0]->child[n++] = other;

    other = malloc(sizeof( struct NTreeNode ));
    other->val = 102;
    other->numberOfChild = 0;
    root->child[0]->child[n++] = other;

    root->child[0]->numberOfChild = n;

    broot = ntree2btree(root);
    inOrderTraversal(broot);

    printTreeNodeInTreeTopology(broot);

    printf("\n");
    printf("Build a binary search tree\n");
    broot = ntree2bstree(root);
    printTreeNodeInTreeTopology(broot);

    // printNTree(root);
    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    return 0;
 }
