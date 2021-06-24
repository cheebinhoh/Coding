/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 * 
 * Breadth first search, both recursive and iterative way
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

/* This is recursive method to do breadth level traversel, the logic is simple:
 * - we maintain two lists:
 * - first level is the Nth level nodes of the tree.
 * - the second level is the N+1th level nodes of the tree.
 *
 * we will print first level node values.
 * we will then copy nodes of 2nd level into 1st level.
 * we will then gather child nodes of refreshed 1st level nodes into 2nd level.
 * then we call the same method recursively. :) 
 */
void breadthLevelTraversal_Internal(struct TreeNode **currentLevelList,
                                    int               currentLevelCnt,
                                    struct TreeNode **nextLevelList,
                                    int               nextLevelCnt )
{
    if ( currentLevelCnt > 0 )
    {
        int i;

        for ( i = 0; i < currentLevelCnt; i++ )
        {
            printf("%d%s", currentLevelList[i]->val, ( i + 1 < currentLevelCnt ) ? ", " : "");
        }

        for ( i = 0; i < nextLevelCnt; i++ )
        {
            currentLevelList[i] = nextLevelList[i];
        }
  
        currentLevelCnt = i;
        nextLevelCnt = 0;
        for ( i = 0; i < currentLevelCnt; i++ )
        {
            struct TreeNode *node = currentLevelList[i];

            if ( NULL != node->left )
            {
                nextLevelList[nextLevelCnt++] = node->left;
            }

            if ( NULL != node->right )
            {
                nextLevelList[nextLevelCnt++] = node->right;
            }
        }

        if ( currentLevelCnt > 0 )
        {
            printf(", ");
        }

        breadthLevelTraversal_Internal(currentLevelList, currentLevelCnt, nextLevelList, nextLevelCnt);
    }
}




/* The breadth level traversal, the iterative version, the logic is simple as none-iterative version:
 * - we maintain two list levels, current and next level.
 * - the loop is going on if current level is NOT exhausted.
 * - the first iterative of the current level, we clear the next level list (empty it).
 * - as we process each node of current level, we gather the next level node into the next level list (appending to what is built up).
 * - if we exhaust the current level list, and there is next level list, we refresh the current level list with next level nodes.
 */
void breadthLevelTraversalIter(struct TreeNode *root)
{
    struct TreeNode *currentLevelList[100];
    struct TreeNode *nextLevelList[100];
    struct TreeNode *node;
    int              currentLevelCnt = 0;
    int              nextLevelCnt = 0;
    int              index = 0;


    if ( NULL == root )
    {
        return;
    }

    currentLevelList[currentLevelCnt++] = root;
    while ( index < currentLevelCnt )
    {
        if ( 0 == index )
        {
            nextLevelCnt = 0;
        }
 
        node = currentLevelList[index];
        if ( NULL != node->left )
        {
            nextLevelList[nextLevelCnt++] = node->left;
        }
 
        if ( NULL != node->right )
        {
            nextLevelList[nextLevelCnt++] = node->right;
        }

        printf("%d", node->val);

        index++;

        if ( index >= currentLevelCnt )
        {
            if ( nextLevelCnt > 0 )
            {
                for ( currentLevelCnt = 0; currentLevelCnt < nextLevelCnt; currentLevelCnt++ )
                {
                    currentLevelList[currentLevelCnt] = nextLevelList[currentLevelCnt];    
                }              

                index = 0;

                printf(", ");
            }
        }
        else 
        {
            printf(", ");
        }
    }

    printf("\n");
}


/* This is API entry to do breadth level traversal, kicker of 
 * recursive method.
 */
void breadthLevelTraversal(struct TreeNode *root)
{
    struct TreeNode *nextLevelList[100];
    struct TreeNode *currentLevelList[100];
    int              nextLevelCnt = 0;
    int              currentLevelCnt = 0;


    if ( NULL == root )
    {
        return;
    }
    else
    {
        currentLevelList[currentLevelCnt++] = root;
        
        if ( NULL != root->left )
        {
            nextLevelList[nextLevelCnt++] = root->left;
        }

        if ( NULL != root->right )
        {
            nextLevelList[nextLevelCnt++] = root->right;
        }

        breadthLevelTraversal_Internal(currentLevelList, currentLevelCnt, nextLevelList, nextLevelCnt);

        printf("\n");
    }
}


/*
 *               0
 *               |
 *      +--------+---------+
 *      1                  2
 *      |                  |
 * +----+----+       //----+----+
 * 3         4                  5
 *                              |
 *                    +---------+---------+
 *                    6                   8
 *                    |                   |
 *              //----+----+          //--+--//
 *                         7 
 */
int main(int argc, char * argv[])
{
    struct TreeNode *root = NULL;
    struct TreeNode *other = NULL;

    root = malloc(sizeof( struct TreeNode));
    root->val = 0;
    root->left = root->right = NULL;

    other = malloc(sizeof( struct TreeNode ));
    other->val = 1;
    other->left = other->right = NULL;
    root->left = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val = 3;
    other->left = other->right = NULL;
    root->left->left = other;
 
    other = malloc(sizeof( struct TreeNode ));
    other->val = 4;
    other->left = other->right = NULL;
    root->left->right = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val = 2;
    other->left = other->right = NULL;
    root->right = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val = 5;
    other->left = other->right = NULL;
    root->right->right = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val = 6;
    other->left = other->right = NULL;
    root->right->right->left = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val = 7;
    other->left = other->right = NULL;
    root->right->right->left->right = other;

    other = malloc(sizeof( struct TreeNode ));
    other->val = 8;
    other->left = other->right = NULL;
    root->right->right->right = other;

    printf("breadth level traverse (recursive) = ");
    breadthLevelTraversal(root);

    printf("breadth level traverse (iterative) = ");
    breadthLevelTraversalIter(root);

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for 
    // this one off program.

    return 0;
 }


