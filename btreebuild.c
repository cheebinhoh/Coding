/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Build binary tree from inorder and postorder list.
 */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"
#include "btreetraverse.h"


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
    enQueue(9, &inorderList);
    enQueue(3, &inorderList);
    enQueue(15, &inorderList);
    enQueue(20, &inorderList);
    enQueue(7, &inorderList);
    printf("Inorder list: ");
    printList(inorderList);

    postorderList = NULL;
    enQueue(9, &postorderList);
    enQueue(15, &postorderList);
    enQueue(7, &postorderList);
    enQueue(20, &postorderList);
    enQueue(3, &postorderList);
    printf("Postorder list: ");
    printList(postorderList);
    printf("\n");

    root = buildBinaryTree(inorderList, postorderList);
    printf("Building binary tree from inorder and postorder list, the tree topology:\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");

    freeList(&inorderList);
    freeList(&postorderList);

    inorderList   = NULL;
    enQueue(2, &inorderList);
    enQueue(1, &inorderList);
    printf("Inorder list: ");
    printList(inorderList);

    postorderList = NULL;
    enQueue(2, &postorderList);
    enQueue(1, &postorderList);
    printf("Postorder list: ");
    printList(postorderList);
    printf("\n");

    root = buildBinaryTree(inorderList, postorderList);
    printf("Building binary tree from inorder and postorder list, the tree topology:\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");

    freeList(&inorderList);
    freeList(&postorderList);

    inorderList   = NULL;
    enQueue(1, &inorderList);
    enQueue(2, &inorderList);
    printf("Inorder list: ");
    printList(inorderList);

    postorderList = NULL;
    enQueue(2, &postorderList);
    enQueue(1, &postorderList);
    printf("Postorder list: ");
    printList(postorderList);
    printf("\n");

    root = buildBinaryTree(inorderList, postorderList);
    printf("Building binary tree from inorder and postorder list, the tree topology:\n");
    printTreeNodeInTreeTopology(root);
    printf("\n");

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // this one off program.

    return 0;
 }
