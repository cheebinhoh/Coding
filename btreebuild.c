/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Build binary tree from inorder and postorder list.
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"
#include "btreetraverse.h"


int main(int argc, char * argv[])
{
    int              i;
    struct TreeNode *root         = NULL;
    int              inorder[]    = { 9, 3, 15, 20, 7 };
    int              postorder[]  = { 9, 15, 7, 20, 3 };
    int              inorder2[]   = { 2, 1 };
    int              postorder2[] = { 2, 1 };
    int              inorder3[]   = { 1, 2 };
    int              postorder3[] = { 2, 1 };
    struct ListNode *inorderList;
    struct ListNode *postorderList;


    inorderList   = NULL;
    enQueueInt(9, &inorderList);
    enQueueInt(3, &inorderList);
    enQueueInt(15, &inorderList);
    enQueueInt(20, &inorderList);
    enQueueInt(7, &inorderList);
    printf("Inorder list: ");
    printListInt(inorderList);

    postorderList = NULL;
    enQueueInt(9, &postorderList);
    enQueueInt(15, &postorderList);
    enQueueInt(7, &postorderList);
    enQueueInt(20, &postorderList);
    enQueueInt(3, &postorderList);
    printf("Postorder list: ");
    printListInt(postorderList);
    printf("\n");

    root = buildBinaryTree(inorderList, postorderList);
    printf("Building binary tree from inorder and postorder list, the tree topology:\n");
    printf("\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");

    freeList(&inorderList);
    freeList(&postorderList);

    inorderList   = NULL;
    enQueueInt(2, &inorderList);
    enQueueInt(1, &inorderList);
    printf("Inorder list: ");
    printListInt(inorderList);

    postorderList = NULL;
    enQueueInt(2, &postorderList);
    enQueueInt(1, &postorderList);
    printf("Postorder list: ");
    printListInt(postorderList);
    printf("\n");

    root = buildBinaryTree(inorderList, postorderList);
    printf("Building binary tree from inorder and postorder list, the tree topology:\n");
    printf("\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");
    printf("\n");

    freeList(&inorderList);
    freeList(&postorderList);

    inorderList   = NULL;
    enQueueInt(1, &inorderList);
    enQueueInt(2, &inorderList);
    printf("Inorder list: ");
    printListInt(inorderList);

    postorderList = NULL;
    enQueueInt(2, &postorderList);
    enQueueInt(1, &postorderList);
    printf("Postorder list: ");
    printListInt(postorderList);
    printf("\n");

    root = buildBinaryTree(inorderList, postorderList);
    printf("Building binary tree from inorder and postorder list, the tree topology:\n");
    printf("\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");
    printf("\n");

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    return 0;
 }
