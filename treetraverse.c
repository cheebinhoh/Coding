#include <stdio.h>
#include <stdlib.h>

// Definition for a binary tree node.
struct TreeNode {

     int val;
     struct TreeNode *left;
     struct TreeNode *right;
};


/* a post order traverse logic:
 *
 * we start at the root, then we process left branch as far as possible, once we exhaust left branch, we process right branch,
 * then we go up one level, and process parent node, after parent node is donee, we process right branch of grand parent node, 
 * and so on.
 *
 * So we do it in post order fashion.
 *
 * How to do it in none-recursive fashion?
 *
 * - we start at the root, if root is not NULL, we go in loop
 * - if root->left is not NULL, we need to process left branch and then backtrack to right branch and the root, so we store
 *   root in rightPendingList, then assign root to be root->left
 * - if root->left is NULL, but root->right is not NULL, then let store the root in topPendingList so we want to backtrack to 
 *   process root, and then we assign right to root
 * - if we have exhausted left or right branch of a root, then we need to bracktrack, so the backtracking is logic as that
 *   -- if topPendingList is not empty, and the top of topPendingList right branch is same as root, then we are at the right branch, then
 *      we print the root->val, and then we bracktrack by assigning top of topPendingList to root, we backtrack top of topPendingList
 *      until the top->right is not same as root
 *   -- once we backtrack to parent node enough, we stawrt to branch to right by lookingg at top of rightPendingList and if the top 
 *      has right branch is NULL, we will backtrace rightPendingList as much as possible.
 */

void postOrderTraversal(struct TreeNode* root)
{
     struct TreeNode *rightPendingList[100];
     struct TreeNode *topPendingList[100];
     int              rightPendingIndex = 0;
     int              topPendingIndex = 0;
     int              count = 0;             // a fail-safe to prevent infinite loop from my miss-coding.

     while ( root != NULL 
             && count < 1000 )
     {
         if ( root->left != NULL ) 
         {
            rightPendingList[rightPendingIndex++] = root;
            root = root->left;    
         }
         else if ( root->right != NULL )
         {
            topPendingList[topPendingIndex++] = root;
            root = root->right;
         }
         else 
         {
             printf("%d, ", root->val);

             while ( topPendingIndex > 0
                     && topPendingList[topPendingIndex - 1]->right == root )
             {
                 root = topPendingList[--topPendingIndex];
                 printf("%d, ", root->val);
             } 

             root = NULL;
             while ( rightPendingIndex > 0 
                     && ( ( root = rightPendingList[--rightPendingIndex] )->right ) == NULL )
	     {
                 printf("%d, ", root->val);
                 root = NULL;
             }
 
             if ( root != NULL )
             { 
                 topPendingList[topPendingIndex++] = root;
                 root = root->right;
             }
         }

         count++;
     }

     printf("\n");
}


void preorderTraversal(struct TreeNode* root)
{
     struct TreeNode *backTrackList[100] = { NULL };
     int backTrackListIndex = 0;

     while ( root != NULL )
     {
          printf( "%d, ", root->val );

          if ( root->left != NULL )
          {
              backTrackList[backTrackListIndex++] = root->right;

              root = root->left;
          }
          else if ( root->right != NULL )
          {
              root = root->right;
          }
          else  
          {
             if ( backTrackListIndex > 0 )
             {
                root = backTrackList[--backTrackListIndex];
             }
             else
             {
                root = NULL;
             }
          }
     }
 
     printf( "\n" );
}


int main( int argc, char * argv[] )
{
    struct TreeNode *root = NULL;
    struct TreeNode *other = NULL;

    root = malloc( sizeof( struct TreeNode) );
    root->val = 0;
    root->left = root->right = NULL;

    other = malloc( sizeof( struct TreeNode ) );
    other->val = 1;
    other->left = other->right = NULL;
    root->left = other;

    other = malloc( sizeof( struct TreeNode ) );
    other->val = 3;
    other->left = other->right = NULL;
    root->left->left = other;
 
    other = malloc( sizeof( struct TreeNode ) );
    other->val = 4;
    other->left = other->right = NULL;
    root->left->right = other;

    other = malloc( sizeof( struct TreeNode ) );
    other->val = 2;
    other->left = other->right = NULL;
    root->right = other;

    other = malloc( sizeof( struct TreeNode ) );
    other->val = 5;
    other->left = other->right = NULL;
    root->right->right = other;

    other = malloc( sizeof( struct TreeNode ) );
    other->val = 6;
    other->left = other->right = NULL;
    root->right->right->left = other;

    other = malloc( sizeof( struct TreeNode ) );
    other->val = 7;
    other->left = other->right = NULL;
    root->right->right->left->right = other;

    other = malloc( sizeof( struct TreeNode ) );
    other->val = 8;
    other->left = other->right = NULL;
    root->right->right->right = other;

    postOrderTraversal(root);

    return 0;
 }

