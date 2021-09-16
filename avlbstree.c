/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * ADT Binary tree (with AVL self-balancing functionality)
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"
#include "btree-internal.h"


void treeRebalanceRecursive(struct TreeNode **parent,
                            struct TreeNode  *root)
{
    int              leftLevel;
    int              rightLevel;
    struct TreeNode *newRoot;
    struct TreeNode *tmp;


    if ( NULL == root )
       return;

    if ( NULL != root->left )
       treeRebalanceRecursive(&(root->left), root->left);

    if ( NULL != root->right )
       treeRebalanceRecursive(&(root->right), root->right);

    rightLevel = determineMaxDepthLevel(root->right, 0);
    leftLevel  = determineMaxDepthLevel(root->left, 0);

    // TODO?
    // - there are significant amount of similarly duplicate logic, where
    //   reordering left and right branches have similar logic, but different
    //   pointer reference (left or right) and relational operator < or >
    //
    // - we can remove that "duplicate" by combinining both blocks into one
    //   generic method that driven by a pass in boolean (left or right)
    //
    // - or we can use macro for text substutition, including >, <, left and right text.
    //
    // - or we just left it be where it is now (duplicated), 1st approach will
    //   alter the original concise logic by taking into account of extra argument, and
    //   it will complicate existing logic.
    //
    //   where else 2nd logic will have to hide thing behind a C macro which is kind
    //   raw and hard to read when cross multiple lines, multi macro is hard to read.
    //
    // In short, I prefer readability of code over saving a few lines, modern compiler
    // is good in generating effecient code.

    if ( ( rightLevel - leftLevel ) >= 2 ) // reorder right branch
    {
        newRoot    = root->right;
        rightLevel = determineMaxDepthLevel(newRoot->right, 0);
        leftLevel  = determineMaxDepthLevel(newRoot->left, 0);


        if ( leftLevel > rightLevel )
        {
            newRoot = newRoot->left;

            tmp = newRoot;
            while ( NULL != tmp->right )
                tmp = tmp->right;

            tmp->right        = root->right;
            root->right->left = NULL;
        }

        tmp = newRoot;
        while ( NULL != tmp->left )
            tmp = tmp->left;

        tmp->left = root;

        root->right = NULL;
        *parent     = newRoot;

        treeRebalanceRecursive(&(newRoot->left), newRoot->left);
    }
    else if ( ( leftLevel - rightLevel ) >= 2 ) // reorder left branch
    {
        newRoot = root->left;

        rightLevel = determineMaxDepthLevel(newRoot->right, 0);
        leftLevel  = determineMaxDepthLevel(newRoot->left, 0);

        if ( rightLevel > leftLevel )
        {
            newRoot = newRoot->right;

            tmp = newRoot;
            while ( NULL != tmp->left )
                tmp = tmp->left;

            tmp->left         = root->left;
            root->left->right = NULL;
        }

        tmp = newRoot;
        while ( NULL != tmp->right )
            tmp = tmp->right;

        tmp->right = root;

        root->left = NULL;
        *parent    = newRoot;

        treeRebalanceRecursive(&(newRoot->right), newRoot->right);
    }
}


struct TreeNode * treeRebalance(struct TreeNode *root)
{
    if ( NULL == root )
        return NULL;

    treeRebalanceRecursive(&root, root);

    return root;
}


struct TreeNode * addTreeNodeAndRebalanceTree(struct TreeNode *root, int val)
{
    root = addTreeNode(root, val);

    return treeRebalance(root);
}


struct TreeNode * delTreeNodeAndRebalanceTree(struct TreeNode *root, int val)
{
    // we do a general tree deletion of node and then we re-balanancing the tree.
    // However re-balancing is not always needed if the delete does not change
    // the height of subtree of the parent node of the to be deleted node does not
    // change.
    //
    // To do that, we need a delTreeNode that return root and also the parent of the
    // to be deleted node.

    root = delTreeNode(root, val);

    return treeRebalance(root);
}


/* We take a bottom up approach that we validate if such tree is AVL balanced,
 * if it is not, then we can return immediately to top of the stack call, else
 * we validate at higher level if the tree is AVL balanced.
 *
 * The other approach is that the AVL TreeNode maintains a left and right count
 * depth level at all time, and rolling the maximum depth level up to higher level
 * and so we only need to check the root level left and right maximum depth gap
 * to decide if the whole tree is AVL
 */
int isTreeNodeBalanced(struct TreeNode *root)
{
    int leftLevel;
    int rightLevel;


    if ( NULL == root )
        return 1;

    if ( NULL != root->left )
        if ( ! isTreeNodeBalanced(root->left) )
            return 0;

    if ( NULL != root->right )
        if ( ! isTreeNodeBalanced(root->right) )
            return 0;

    rightLevel = determineMaxDepthLevel(root->right, 0);
    leftLevel  = determineMaxDepthLevel(root->left, 0);

    return  ( ( leftLevel - rightLevel ) < 2 )
            && ( ( rightLevel - leftLevel ) < 2 );
}
