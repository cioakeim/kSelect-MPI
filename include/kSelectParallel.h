/* kSelectParallel.h: 
 * This module contains the parallel implementation of the kSelect algorithm, using Open MPI 5.0.
*/
#ifndef K_SELECT_PARALLEL_H
#define K_SELECT_PARALLEL_H

#include "kSelectSequential.h"


/* Main Parallel algorithm.
 * Returns the value of the k-th largest element of a. 
 */
int kSelectParallel(char *array_filename,int k); 

/* Master function that selects a pivot based on the current array bounds*/ 
int selectPivot(int ip, int jp, int *a);

/* Calculates next mode based on the total amount of less than and pivot counts.
 * Relative_k is updated (passed by reference)
 */
enum mode decideNextMode(int less_than_total_count,int pivot_total_count,int *relative_k);

/* Simple initiation*/
INDICES initialIndices(int a_size);

/* Updating indices for each loop iteration to set the working part of the array based on the 
 * given mode.
 */
void updateIndices(INDICES *p, enum mode mode);

#endif
