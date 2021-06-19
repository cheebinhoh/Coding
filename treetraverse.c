/* Copyright © 2021 Chee Bin HOH. All rights reserved.
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


/* a post order traverse logic:
 *
 * we start at the root, then we process left branch as far as possible, once we exhaust left branch, we process right branch,
 * then we go up one level, and process parent node, after parent node is done, we process right branch of grand parent node, 
 * and so on.
 *
 * So we do it in post order fashion.
 *
 * How to do it in none-recursive fashion?
 *
 * - we do none-recursive loop way to traverse the tree
 * - we start at the root, if root is not NULL, we go in loop
 * - if root->left is not NULL, we need to process left branch and then backtrack to right branch and then parent node of the branches, so we store
 *   root (parent node) in rightPendingList, then assign root to be root->left
 * - if root->left is NULL, but root->right is not NULL, then let store the root in topPendingList so we want to backtrack to 
 *   process the node (after processing right branch), and then we assign right branch to root
 * - if we have exhausted left or right branches of a node (root), then we need to bracktrack, so the backtracking is logic as that
 *   -- if topPendingList is not empty, and the top (of topPendingList) right branch is same as root, then the root is at the right branch of top of 
 *      topPendingList, then we print the root->val, and we bracktrack by assigning top of topPendingList to root, so eventually we are backtracking
 *      from right branch back up to top, until the top->right is not same as root
 *   -- once we backtrack to parent node enough, we start to process right branch by looking at top of rightPendingList and if the top 
 *      has right branch is NULL, we will backtrack further on rightPendingList until we exhaust it or a top that its right branch is not NULL.
 *
 * rightPendingList is to store node that its right branch is pending to be processed
 * topPendingList is to store node that its left branch has processed, right branch is now in process, but its node is not yet processed
 *
 */

void postOrderTraversal(struct TreeNode* root)
{
     struct TreeNode *rightPendingList[100];
     struct TreeNode *topPendingList[100];
     int              rightPendingIndex = 0;
     int              topPendingIndex = 0;
     int              count = 0;             // a fail-safe to prevent infinite loop from my miss-coding.
     int              start = 0;


     while ( NULL != root 
             && count < 1000 )
     {
         if ( NULL != root->left ) 
         {
            rightPendingList[rightPendingIndex++] = root;
            root = root->left;    
         }
         else if ( NULL != root->right )
         {
            topPendingList[topPendingIndex++] = root;
            root = root->right;
         }
         else 
         {
             printf("%s%d", ( 0 == start ? "" : ", " ),  root->val);
             start = 1;

             while ( topPendingIndex > 0
                     && topPendingList[topPendingIndex - 1]->right == root )
             {
                 root = topPendingList[--topPendingIndex];
                 printf(", %d", root->val);
             } 

             root = NULL;
             while ( rightPendingIndex > 0 
                     && ( ( root = rightPendingList[--rightPendingIndex] )->right ) == NULL )
             {
                 printf(", %d", root->val);
                 root = NULL;
             }
 
             if ( NULL != root )
             { 
                 topPendingList[topPendingIndex++] = root;
                 root = root->right;
             }
         }

         count++;
     }

     if ( count > 0 )
     { 
         printf("\n");
     }
}


/* a in order traversal logic:
 *
 * we start at the root, and if there is a left branch, we remember the root in topPendingList for backtracking and then traverse to 
 * left branch by assigning it as new root.
 *
 * if there is no right branch, then we repeatedly backtrack from topPendingList by:
 * - printing the value of the node we pop out from topPendingList
 * - exit the backtracking loop if the pop out node has right branch, else keep popping out
 */

void inOrderTraversal(struct TreeNode *root)
{
    struct TreeNode *topPendingList[100];
    int              topPendingListIndex = 0;
    int              count = 0;
    int              start = 0; 


    while ( NULL != root 
            && count < 1000 )
    {
        count++;

        if ( NULL != root->left )
        {
            topPendingList[topPendingListIndex++] = root;
            root = root->left;
        }
        else
        {
            printf("%s%d", ( 0 == start ? "" : ", " ),  root->val);
            start = 1;

            if ( NULL != root->right )
            {
                root = root->right;
            }
            else if ( topPendingListIndex > 0 ) 
            { 
                do 
                {
                    root = topPendingList[--topPendingListIndex];
                    printf(", %d", root->val);
     
                    root = root->right;
                } while ( NULL == root 
                          && topPendingListIndex > 0 );
            }
            else
            {
                root = NULL;
            }
        }
    }

    if ( count > 0 )
    {
        printf("\n");
    }
}


/* A pre order traversal logic:
 *
 * We start at the root, print the value, traverse the left branch and exhaust left branch as far as possible, then traverse right branch,
 * then backtrack one level for right back at higher level.
 *
 * Before we traverse left branch, we add right branch (if not NULL) to the rightPendingList for backtrack later.
 */
void preOrderTraversal(struct TreeNode *root)
{
     struct TreeNode *rightPendingList[100];
     int              rightPendingListIndex = 0;
     int              count = 0;


     while ( NULL != root 
             && count < 1000 )
     {
          count++;
          printf("%s%d", count > 1 ? ", " : "",  root->val);

          if ( NULL != root->left )
          {
              if ( NULL != root->right )
              {
                  rightPendingList[rightPendingListIndex++] = root->right;
              }

              root = root->left;
          }
          else if ( NULL != root->right )
          {
              root = root->right;
          }
          else  
          {
             if ( rightPendingListIndex > 0 )
             {
                root = rightPendingList[--rightPendingListIndex];
             }
             else
             {
                root = NULL;
             }
          }
     }
 
     if ( count > 0 )
     {
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

    printf("pre order  = ");
    preOrderTraversal(root);
   
    printf("post order = ");  
    postOrderTraversal(root);

    printf("in order   = ");
    inOrderTraversal(root);

    // I do not care about freeing malloced memory, OS will take care of freeing heap that is part of process for
    // one of program.

    return 0;
 }

