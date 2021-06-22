/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Build binary tree from inorder and postorder list.
 */

#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "treetraverse.h"


/* Find a parent node from a tree for a node
 */
struct TreeNode * findParent(struct TreeNode *root,
                             struct TreeNode *node)
{
    struct TreeNode *parent = NULL;


    if ( NULL == root 
         || NULL == node )
    {
        return NULL;
    }

    if ( root == node )
    {
        return node;
    }

    if ( root->left == node 
         || root->right == node )
    {
        return root;
    }

    if ( NULL != root->left )
    {
        parent = findParent(root->left, node);
    }   

    if ( NULL == parent)
    {
        if ( NULL != root->right )
        {
            parent = findParent(root->right, node);
        }
    }

    return parent;
}


/* The logic is simple:
 * - we can build a binary tree from post order list much easily as a reverse postorder tree resemble the 
 *   top-down fashion of the tree with right branch lean. so a binary tree postorder is left-lean, a reverse
 *   of it is right branch lean.
 *
 * - if a post order node value appears on right position of the in order list, then the value is part of the
 *   right branch of previous node.
 *
 * - if a post order node value appears on left positio of the in order list, then the value is part of the 
 *   left branch of _parent_ node of previous node.
 */ 
struct TreeNode * buildTree(int inorder[], 
                            int postorder[],
                            int cnt)
{
    struct TreeNode *root, *prev, *new;
    int              i;
    int              tmp;
    int              pos;


    root = prev = NULL;
    for ( i = 0; i < cnt / 2; i++ )
    {
        tmp = postorder[cnt - 1 - i];
        postorder[cnt - 1 - i] = postorder[i];
        postorder[i] = tmp;
    }

    for ( i = 0; i < cnt; i++ )
    {
        new = malloc(sizeof( struct TreeNode ));
        new->left = new->right = NULL;
        new->val = postorder[i];

        if ( NULL == root )
        {
            root = prev = new;
        }
        else
        {
            pos = 0;
            while ( pos < cnt
                    && inorder[pos] != postorder[i] )
            {
                pos++;
            }

            if ( pos >= i )
            {
                prev->right = new;
                prev = new;
            }
            else
            {
                prev = findParent(root, prev);
                prev->left = new;              
            }
        }
    }

    return root;
}


/*
 *               3
 *               |
 *      +--------+---------+
 *      9                  20
 *                         |
 *                    +----+----+
 *                    15        7
 */
int main(int argc, char * argv[])
{
    struct TreeNode *root = NULL;
    int              inorder[]    = { 9, 3, 15, 20, 7 };
    int              postorder[]  = { 9, 15, 7, 20, 3 };
    int              inorder2[]   = { 2, 1 };
    int              postorder2[] = { 2, 1 };
    int              inorder3[]   = { 1, 2 };
    int              postorder3[] = { 2, 1 };


    root = buildTree(inorder, postorder, sizeof(inorder) / sizeof(inorder[0]));

    printf("in order    = ");
    inOrderTraversal(root);      

    printf("post order  = ");
    postOrderTraversal(root);   

    root = buildTree(inorder2, postorder2, sizeof(inorder2) / sizeof(inorder2[0]));

    printf("in order2   = ");
    inOrderTraversal(root);

    printf("post order2 = ");
    postOrderTraversal(root);

    root = buildTree(inorder3, postorder3, sizeof(inorder3) / sizeof(inorder3[0]));

    printf("in order3   = ");
    inOrderTraversal(root);

    printf("post order3 = ");
    postOrderTraversal(root);

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    return 0;
 }


