/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Coding test questions from Apple (at least internet claim it :)) and many others.
 *
 * https://www.codinginterview.com/apple-interview-questions, I also include
 * questions that is similar I come across from friend or online.
 *
 * All answers are done by me with some assumption sometimes to make it easy for me :)
 * I do some reading about some topic to refresh my knowledge about certain theory, like
 * AVL before I start coding it.
 *
 * Happy coding!
 */

#include <stdio.h>
#include <string.h>
#include "btree.h"


void printIntegerArray(int array[], int size)
{
    int i;


    for ( i = 0; i < size - 1; i++ )
       printf("%d, ", array[i]);

    if ( size >= 1 )
       printf("%d\n", array[size - 1]);
}

/* Test 1:
 *
 * you are given an array of positive numbers from 1 to n, such that all numbers
 * from 1 to n are present except one number x. You have to find x. The input array is not sorted.
 * Look at the below array and give it a try before checking the solution
 */
int findMissingNumberInTheArray(int array[], int size, int n)
{
    int sum;
    int expectedSum;
    int i;


    expectedSum = 0;
    for ( i = 1; i <= n; i++ )
        expectedSum += i;

    sum = 0;
    for ( i = 0; i < size; i++ )
        sum += array[i];

    return ( expectedSum - sum );
}

void runFindMissingNumberInTheArray(void)
{
    int array1[] = { 3, 7, 1, 2, 8, 4, 5 };


    printf("Test 1: find the missing number in the array.\n");
    printf("\n");

    printf("The integer array is ");
    printIntegerArray(array1, sizeof(array1) / sizeof(array1[0]));
    printf("The missing integer is %d\n",
           findMissingNumberInTheArray(array1, sizeof(array1) / sizeof(array1[0]), 8));
    printf("\n");
}


/*
 * Test 2:
 *
 * Given an array of integers and a value, determine if there are any two integers in
 * the array whose sum is equal to the given value. Return true if the sum exists and
 * return false if it does not.
 */

void print2NumberSumToValue(int array[], int size, int val)
{
    int              i;
    int              remaining;
    struct TreeNode *root;
    struct TreeNode *node;
    struct TreeNode *foundList;


    root = NULL;
    for ( i = 0; i < size; i++ )
        root = addTreeNode(root, array[i]);

    node      = NULL;
    foundList = NULL;
    for ( i = 0; i < size; i++ )
    {
         node = findTreeNode(foundList, array[i]);
         if ( NULL != node )
             continue;

         remaining = val - array[i];

         node = findTreeNode(root, remaining);
         if ( NULL != node
              && node->val != array[i])
         {
             foundList = addTreeNode(foundList, array[i]);
             foundList = addTreeNode(foundList, node->val);

             printf("[%d, %d] ", array[i], node->val);
         }
    }

    freeTreeNode(root);
    freeTreeNode(foundList);

    printf("\n");
}

void runDeteremineIf2NumberSumToValue(void)
{
    int array1[] = { 5, 7, 1, 2, 8, 4, 3 };
    int found;


    printf("Test 2: determine if the sum of two integers is equal to the given value.\n");
    printf("\n");

    printf("The integer array is ");
    printIntegerArray(array1, sizeof(array1) / sizeof(array1[0]));
    printf("The list of two numbers added to 10 is ");
    print2NumberSumToValue(array1, sizeof(array1) / sizeof(array1[0]), 10);
    printf("\n");
}

/* Test 3:
 *
 * Given a dictionary of words and a large input string. You have to find out whether the
 * input string can be completely segmented into the words of a given dictionary.
 */

int determineIfStringCanBeSegmented(char *dictionary[], char *s)
{
    int i;
    int len;
    int can;


    for ( i = 0; NULL != dictionary[i]; i++ )
    {
        len = strlen(dictionary[i]);

        if ( strncmp(dictionary[i], s, len ) == 0 )
        {
            if ( '\0' == s[len] )
                return 1;
            else if ( determineIfStringCanBeSegmented(dictionary, s + len) )
                return 1;

            // recursive method can be expensive and depending on the string to be segemented,
            // we can be stack overflow, another way is to maintain backtracking explicitly by
            // string the previous position in a list or array.
        }
    }

    return 0;
}

void runStringSegmentation(void)
{
    char *dictionary[] = { "apple", "pear", "pie", NULL };
    char  s1[]         = "applepie";
    char  s2[]         = "applepeer";
    int   i;


    printf("Test 3: given a dictionary of words and a large input string. You have to find out "
            "whether the input string can be completely segmented into the words of a given dictionary.\n");
    printf("\n");

    printf("The dictionary words are: ");
    for ( i = 0; NULL != dictionary[i]; i++ )
        printf("\"%s\" ", dictionary[i]);

    printf("\n");

    if ( determineIfStringCanBeSegmented(dictionary, s1) )
         printf("The string \"%s\" can be segmented\n", s1);
    else
         printf("The string \"%s\" can not be segmented\n", s1);

    if ( determineIfStringCanBeSegmented(dictionary, s2) )
         printf("The string \"%s\" can be segmented\n", s2);
    else
         printf("The string \"%s\" can not be segmented\n", s2);

    printf("\n");
}



// the starting point...
int main(int argc, char *argv[])
{
    runFindMissingNumberInTheArray();
    printf("\n");

    runDeteremineIf2NumberSumToValue();
    printf("\n");

    runStringSegmentation();
    printf("\n");

    return 0;
}
