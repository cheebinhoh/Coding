/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Find out if a binar tree is a substree of another binary tree.
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"
#include "llist.h"


int main(int argc, char * argv[])
{
    struct TreeNode *root  = NULL;
    struct TreeNode *other = NULL;
    struct TreeNode *root2 = NULL;
    struct ListNode *prel;
    struct ListNode *postl;
    struct ListNode *inl;
    int              tmp;


    root        = malloc(sizeof( struct TreeNode ));
    root->val   = 0;
    root->left  = NULL;
    root->right = NULL;

    other        = malloc(sizeof( struct TreeNode ));
    other->val   = 1;
    other->left  = NULL;
    other->right = NULL;
    root->left   = other;

    other            = malloc(sizeof( struct TreeNode ));
    other->val       = 3;
    other->left      = NULL;
    other->right     = NULL;
    root->left->left = other;

    other             = malloc(sizeof( struct TreeNode ));
    other->val        = 4;
    other->left       = NULL;
    other->right      = NULL;
    root->left->right = other;

    other        = malloc(sizeof( struct TreeNode ));
    other->val   = 2;
    other->left  = NULL;
    other->right = NULL;
    root->right  = other;

    other              = malloc(sizeof( struct TreeNode ));
    other->val         = 5;
    other->left        = NULL;
    other->right       = NULL;
    root->right->right = other;

    other                    = malloc(sizeof( struct TreeNode ));
    other->val               = 6;
    other->left              = NULL;
    other->right             = NULL;
    root->right->right->left = other;

    other                           = malloc(sizeof( struct TreeNode ));
    other->val                      = 7;
    other->left                     = NULL;
    other->right                    = NULL;
    root->right->right->left->right = other;

    other                     = malloc(sizeof( struct TreeNode ));
    other->val                = 8;
    other->left               = NULL;
    other->right              = NULL;
    root->right->right->right = other;

    printf("The tree topology:\n");
    printTreeNodeInTreeTopology(root);

    printf("\n");
    inl = getInOrderList(root);
    printf("In order list, length = %d\n", getListLength(inl));
    printList(inl);
    printf("\n");

    prel = getPreOrderList(root);
    printf("Pre order list, length = %d\n", getListLength(prel));
    printList(prel);
    printf("\n");

    postl = getPostOrderList(root);
    printf("Post order list, length = %d\n", getListLength(postl));
    printList(postl);
    printf("\n");


    root2        = malloc(sizeof(struct TreeNode ));
    root2->val   = 1;
    root2->left  = NULL;
    root2->right = NULL;

    other        = malloc(sizeof(struct TreeNode ));
    other->val   = 3;
    other->right = NULL;
    other->left  = NULL;
    root2->left  = other;

    other        = malloc(sizeof(struct TreeNode ));
    other->val   = 4;
    other->right = NULL;
    other->left  = NULL;
    root2->right = other;

    printf("\n");
    printf("The 2nd tree topology:\n");
    printTreeNodeInTreeTopology(root2);

    printf("\n");
    inl = getInOrderList(root2);
    printf("In order list, length = %d\n", getListLength(inl));
    printList(inl);
    printf("\n");

    prel = getPreOrderList(root2);
    printf("Pre order list, length = %d\n", getListLength(prel));
    printList(prel);
    printf("\n");

    postl = getPostOrderList(root2);
    printf("Post order list, length = %d\n", getListLength(postl));
    printList(postl);
    printf("\n");

    if ( isTreeSubTree(root, root2) )
    {
        printf("The 2nd tree is sub tree of first tree\n");
    }
    else
    {
        printf("The 2nd tree is not sub tree of first tree\n");
    }

    printf("\n");
    printf("\n");
    printf("Adjusting 2nd tree by by swapping left and right branch value\n");
    tmp               = root2->left->val;
    root2->left->val  = root2->right->val;
    root2->right->val = tmp;

    printf("\n");
    printf("The adjusted 2nd tree topology:\n");
    printTreeNodeInTreeTopology(root2);

    printf("\n");
    inl = getInOrderList(root2);
    printf("In order list, length = %d\n", getListLength(inl));
    printList(inl);
    printf("\n");

    prel = getPreOrderList(root2);
    printf("Pre order list, length = %d\n", getListLength(prel));
    printList(prel);
    printf("\n");

    postl = getPostOrderList(root2);
    printf("Post order list, length = %d\n", getListLength(postl));
    printList(postl);
    printf("\n");

    if ( isTreeSubTree(root, root2) )
    {
        printf("The adjusted 2nd tree is sub tree of first tree\n");
    }
    else
    {
        printf("The adjusted 2nd tree is not sub tree of first tree\n");
    }

    printf("\n");
    printf("\n");
    printf("Adjusting 2nd tree by extending val 3 with left child 5\n");

    // swap it back
    tmp               = root2->left->val;
    root2->left->val  = root2->right->val;
    root2->right->val = tmp;

    other             = malloc(sizeof(struct TreeNode ));
    other->val        = 5;
    other->right      = NULL;
    other->left       = NULL;
    root2->left->left = other;

    printf("\n");
    printf("The adjusted 2nd tree topology:\n");
    printTreeNodeInTreeTopology(root2);

    printf("\n");
    inl = getInOrderList(root2);
    printf("In order list, length = %d\n", getListLength(inl));
    printList(inl);
    printf("\n");

    prel = getPreOrderList(root2);
    printf("Pre order list, length = %d\n", getListLength(prel));
    printList(prel);
    printf("\n");

    postl = getPostOrderList(root2);
    printf("Post order list, length = %d\n", getListLength(postl));
    printList(postl);
    printf("\n");

    if ( isTreeSubTree(root, root2) )
    {
        printf("The adjusted 2nd tree is sub tree of first tree\n");
    }
    else
    {
        printf("The adjusted 2nd tree is not sub tree of first tree\n");
    }

    printf("\n");

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    return 0;
 }
