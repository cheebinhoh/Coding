/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Check if tree is sysmetric that right branch of root is mirror of left branch.
 */

#include <stdio.h>
#include <stdlib.h>


/* Definition for a binary tree node.
 */
struct TreeNode {

     int val;
     struct TreeNode *left;
     struct TreeNode *right;
};


/* This method will linearize the tree into a list either left or right lean.
 */
void linearTree(struct TreeNode  *root, 
                int               left,
                struct TreeNode **list,
                int              *listCnt)
{
    if ( NULL == root )
    {
        list[*listCnt] = NULL;
        *listCnt = *listCnt + 1;
    }
    else 
    {
        list[*listCnt] = root;
        *listCnt = *listCnt + 1;
  
        if ( left ) 
        {
          linearTree(root->left, 
                     left,
                     list,
                     listCnt);

          linearTree(root->right, 
                     left,
                     list,
                     listCnt);
        }
        else
        {
          linearTree(root->right,
                     left,
                     list,
                     listCnt);
          
          linearTree(root->left,
                     left,
                     list,
                     listCnt);
        }
    }
}


/* This is API front end to derive if a tree is sysmetric, a tree is sysmetric if we linearize
 * the tree into a list left or right lean, and it has same result.
 */
int treeIsSysmetric(struct TreeNode *root)
{
    if ( NULL == root )
    {
        return 1;
    }
    else 
    {
        struct TreeNode *leftList[100];
        struct TreeNode *rightList[100];
        int              leftListCnt = 0;
        int              rightListCnt = 0; 
        int              i;

        linearTree(root, 1, leftList, &leftListCnt);
        linearTree(root, 0, rightList, &rightListCnt);

        printf("left  = " );
        for ( i = 0; i < leftListCnt; i++ )
        {
            struct TreeNode *node;

            node = leftList[i];
            if ( NULL == node )
            {
                printf("-%s", i + 1 >= leftListCnt ? "" : ", "); 
            }
            else
            {
                printf("%d, ", node->val);
            }
        }

        printf("\n");

        printf("right = ");
        for ( i = 0; i < rightListCnt; i++ )
        {
            struct TreeNode *node;

            node = rightList[i];
            if ( NULL == node )
            {
                printf("-%s", i + 1 >= rightListCnt ? "" : ", ");
            }
            else
            {
                printf("%d, ", node->val);
            }
        }
        
        printf("\n");

        if ( leftListCnt != rightListCnt )
        {
            return 0;
        }
        else
        {
            for ( i = 0; i < leftListCnt; i++ )
            {
                if ( NULL != leftList[i]
                     && NULL != rightList[i] )
                {
                    if ( leftList[i]->val != rightList[i]->val )
                    {
                        return 0;
                    }   
                }
                else if ( leftList[i] != rightList[i] ) 
                {
                    return 0;
                }
            }

            return 1;
        }
    }
}

/*
 *               0
 *               |
 *      +--------+---------+
 *      1                  1
 *      |                  |
 * +---------+        +---------+
 * 2         3        3*        2
 *
 * remark *: 3 will be deleted in 2nd sysmetric check below
 */
int main(int argc, char * argv[])
{
    struct TreeNode *root = NULL;
    struct TreeNode *other = NULL;


    root = malloc(sizeof( struct TreeNode ));
    root->val = 0;
    root->left = root->right = NULL;

    other = malloc(sizeof( struct TreeNode ));
    other->val = 1;
    other->left = other->right = NULL;
   
    root->left = other;
 
  
    other = malloc(sizeof( struct TreeNode ));
    other->val = 1;
    other->left = other->right = NULL;
    
    root->right = other;


    other = malloc(sizeof( struct TreeNode ));
    other->val = 2;
    other->left = other->right = NULL;

    root->right->right = other;


    other = malloc(sizeof( struct TreeNode ));
    other->val = 2;
    other->left = other->right = NULL;

    root->left->left = other;


    other = malloc(sizeof( struct TreeNode ));
    other->val = 3;
    other->left = other->right = NULL;

    root->left->right = other;


    other = malloc(sizeof( struct TreeNode ));
    other->val = 3;
    other->left = other->right = NULL;

    root->right->left = other;

    printf("Is sysmetric = %d\n", treeIsSysmetric(root));
    printf("\n");

    root->right->left = NULL;
    
    printf("Is sysmetric (after setting root->right->left to NULL) = %d\n", treeIsSysmetric(root));

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    return 0;
 }

