#include <stdio.h>
#include <stdlib.h>

// Definition for a binary tree node.
struct TreeNode {

     int val;
     struct TreeNode *left;
     struct TreeNode *right;
};


void postorderTraversal(struct TreeNode* root)
{
     struct TreeNode *rightPendingList[100] = { NULL };
     struct TreeNode *topPendingList[100] = { NULL };
     int              rightPendingIndex = 0;
     int              topPendingIndex = 0;
     int              count = 0;

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

    postorderTraversal(root);

    return 0;
 }

