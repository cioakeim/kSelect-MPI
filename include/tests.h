#ifndef TESTS_H
#define TESTS_H
/* Testing file used to verify the correctness of the implemented algorithms*/ 

/* Insertion sort is used to get the sorted array and then access its k-th element*/ 
void insertion_sort(int *arr, int n); 

/* This verifies the correctness of the sequential kSelect algorithm.
*  Arrays are created from size 1 to a_max_size, while the desired k is looped from 0 to a.size-1. 
*  Insertion sort is used to verify the result.
*  Returns number of failed tests.
*/
int testSeqSelect(int a_max_size,int a_max_val);

#endif
