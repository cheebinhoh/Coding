/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Quick short, shell sort, insertion sort, bubble sort and binary search
 */

#include <stdio.h>


#define ARRAY_COUNT(array)  ( sizeof((array)) / sizeof((array)[0]) )


void bubbleSort(int list[], int size);
void insertionSort(int list[], int size);
void shellSort(int list[], int size);
void quickSort(int list[], int size);
int binarySearch(int list[], int size, int value);
int isSorted(int list[], int size);


int main(int argc, char *argv[])
{
    //             0  1  2  3  4  
    int list[] = { 5, 4, 3, 1, 2 };
    int index;
 

    printf("-- is sorted = %d\n", isSorted( list, ARRAY_COUNT( list ) ) );
    shellSort(list, ARRAY_COUNT( list ));
    for ( index = 0; index < ARRAY_COUNT( list ); index++ )
    {
        printf("%d ", list[index]);
    }

    printf("\n"); 

    printf("-- is sorted = %d\n", isSorted( list, ARRAY_COUNT( list ) ));

    return 0;
}


int isSorted(int list[], int size)
{
    int result = 1;
    int i;


    for ( i = 1; i < size - 1; i++ )
    {
        if ( list[i - 1] > list[i] )
        {
            result = 0;

            break;
        }
    }

    return result;
}


/* Quick sort
 *
 * The algorithm is that:
 * - the right most element is picked as a pivot.
 * - then we compare pivot element to the left indexed element (start from the left).
 * --- if the left element is smaller than the pivot element (from right), we increase the index of left element by 1.
 * --- if the left element is larger than the pivot element, we move the left element to the pivot element to the left indexed
 *     element position, we move the pivot element into the left position of the pivot element, and we reloate original 
 *     left indexed element to the right of the new pivot position
 * --- we repeat above as long as left index is smaller than pivot position
 * - once we stop that, we know that everything to the left of the pivot will be smaller, and everything to the right will be
 *   larger than pivot element.
 * - then we recursively invoke quick sort on left and right elements of the pivot.
 */
void quickSort(int list[], int size)
{
    int pivot = size - 1;
    int i     = 0;
    int tmp;


    while ( i < pivot )
    {
        if ( list[i] <= list[pivot] )
        {
            i++;
        }
        else
        {
            // pivot is moved to position pivot - 1
            // original item at position pivot - 1 is moved to the position ith
            // original item at position ith is moved to the original pivot position

            tmp = list[i];
            list[i] = list[pivot - 1];
            list[pivot - 1] = list[pivot];
            list[pivot] = tmp;   

            pivot--;
        }
    }

    if ( pivot > 1 )
    {
        quickSort(list, pivot)  ;
    }

    if ( size - pivot - 1 > 1 )
    {
        quickSort(&list[pivot + 1], size - pivot - 1);
    }
}


/* Shell sort
 *
 * Shell sort is like bubble sort and compare a pair of elements and sort them
 * into proper ordering, however instead of compare adjacent elements, shell sort
 * will start off with a pair of elements separated apart by bigger gap, and then
 * repeatedly decrease the gap and then eventually sort adjcent pair, bubble reduces
 * the # of swap that happened.
 */
void shellSort(int list[], int size)
{
    int gap;
    int j;
    int i;
    int tmp;
    int count = 0;


    for ( gap = size / 2; gap > 0; gap = gap / 2 )
    {
        for ( j = gap; j < size; j++ )
        {
            for ( i = j - gap; i >= 0 && list[i] > list[i + gap]; i = i - gap )
            {
                tmp = list[i];
                list[i] = list[i + gap];
                list[i + gap] = tmp;
            }
        }
    }    

    printf("total swap = %d\n", count);
}


/* Insertion sort
 *
 * loop through each item via i
 * compare all elements before i and insert i in proper position
 */
void insertionSort(int list[], int size)
{
    int i;
    int j;
    int tmp;
    int count = 0;


    for ( i = 1; i < size; i++ )
    {
        tmp = list[i];
        
        for ( j = i - 1; j >= 0 && list[j] > tmp; j-- )
        {
          count++;
          list[j + 1] = list[j];
          list[j] = tmp;
        }
    }

    printf("total swap = %d\n", count);
}


/* Bubbles sort
 */
void bubbleSort(int list[], int size)
{
    int i;
    int j;
    int tmp;


    for ( i = 0; i < size - 1; i++ )
    {
        for ( j = 0; j < size - 1 - i; j++ )
        {
            if ( list[j] > list[j + 1] )
            {
                tmp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = tmp;
            }   
        }
    }
}


/* Binary search
 */
int binarySearch(int list[], int size, int value)
{
    int head   = 0;
    int tail   = size - 1;
    int middle = ( head + tail ) / 2;


    while ( head <= tail
            && list[middle] != value )
    {
       if ( value < list[middle] )
       {
          tail = middle - 1;
       }
       else
       {
          head = middle + 1;
       }
 
       middle = ( head + tail ) / 2;
    }

    return list[middle] == value ? middle : -1;
}
