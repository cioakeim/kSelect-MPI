#ifndef TESTS_H
#define TESTS_H
/* Testing file used to verify the correctness of the implemented algorithms*/ 

#include <stdint.h>

/* Insertion sort is used to get the sorted array and then access its k-th element*/ 
void insertion_sort(uint32_t *arr, int n); 

/* This verifies the correctness of the sequential kSelect algorithm.
*  Arrays are created from size 1 to a_max_size, while the desired k is looped from 0 to a.size-1. 
*  Insertion sort is used to verify the result.
*  Returns number of failed tests.
*/
int testSeqSelect(int a_max_size,uint32_t a_max_val);

/*  This verifies the correctness of kSelectParallel using random arrays and insertion_sort.
*   Arrays are created from size 1 to max_size, while the desired k is looped from 0 to a.size-1.
*   Each array is written in the localData directory in a temp.txt file in the format of arrayParse.
*   Returns the number of failed tests.
*/
int testParallelSelect(uint64_t a_max_size,uint32_t a_max_val);

#endif
