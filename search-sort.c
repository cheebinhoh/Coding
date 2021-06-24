#include <stdio.h>

#define ARRAY_COUNT(array)  ( sizeof((array)) / sizeof((array)[0]) )


int binarySearch( int list[], int size, int value );
void bubbleSort( int list[], int size );
void insertionSort( int list[], int size );
void shellSort( int list[], int size );
void quickSort( int list[], int size );

int isSorted( int list[], int size );

int main(int argc, char *argv[])
{
    //             0  1  2  3  4  5
    int list[] = { 5, 4, 3, 1, 2 };
    int index;
 
    printf("-- is sorted = %d\n", isSorted( list, ARRAY_COUNT( list ) ) );
    shellSort( list, ARRAY_COUNT( list ) );
    for ( index = 0; index < ARRAY_COUNT( list ); index++ )
    {
        printf("%d ", list[index]);
    }

    printf("\n"); 

    printf("-- is sorted = %d\n", isSorted( list, ARRAY_COUNT( list ) ) );

    return 0;
}


int isSorted( int list[], int size )
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

/* The algorithm is that:
 * - the right most element is picked as a pivot.
 * - then we move the pivot in the right position that all elements to the left are less than it, and 
 *   all elements to its right is more than it. 
 * - then we recursively invoke quick sort on left and right elements of the pivot.
 */
void quickSort( int list[], int size )
{
    int pivot = size - 1;
    int i = 0;
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
	quickSort( list, pivot )  ;
    }

    if ( size - pivot - 1 > 1 )
    {
        quickSort( &list[pivot + 1], size - pivot - 1 );
    }
}

void shellSort( int list[], int size )
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


    /*
    for ( gap = size / 2; gap > 0; gap = gap / 2 )
    {
	for ( j = 0; j + gap < size; j++ )
	{
	    for ( i = j; i + gap < size; i += gap )
	    {
		if ( list[i] > list[i + gap] )
		{
	            count++;
	            tmp = list[i];
		    list[i] = list[i + gap];
	  	    list[i + gap] = tmp;
		}
	    }               
	}
    }
    */

    printf( "total swap = %d\n", count );
}


/* loop through each item via i
 * compare all elements before i and insert i in proper position
 */
void insertionSort( int list[], int size )
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

    printf( "total swap = %d\n", count );
}

void bubbleSort( int list[], int size )
{
    int i, j;
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

int binarySearch( int list[], int size, int value )
{
    int head = 0;
    int tail = size - 1;
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
