/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Test linked list (single)
 */

#include <stdio.h>
#include "llist.h"


int main(int argc, char *argv[])
{
    struct ListNode *start = NULL;
    struct ListNode *end   = NULL;


    addListNode(3, &start, &end);
    addListNode(2, &start, &end);
    addListNode(4, &start, &end);
    addListNode(1, &start, &end);

    printf("The list of integer is: ");
    printListNode(start);
    printf("\n");

    printf("After deleeting 2\n");
    delListNode(2, &start, &end);
    printListNode(start);
    printf("\n");

    return 0;
}
