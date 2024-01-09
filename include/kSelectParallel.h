/* kSelectParallel.h: 
 * This module contains the parallel implementation of the kSelect algorithm, using Open MPI 5.0.
*/
#ifndef K_SELECT_PARALLEL_H
#define K_SELECT_PARALLEL_H

#include "kSelectSequential.h"
#include "arrayParsing.h"
#include <stdint.h>
#include <stdbool.h>


/* Main Parallel algorithm.
 * Returns the value of the k-th largest element of a. 
 */
uint32_t kSelectParallel(ARRAY array,uint64_t k); 

/* Auxilliary function for gathering*/
uint32_t gatherToRootAndFinish(uint32_t *array,int world_rank,int world_size, INDICES indices,uint64_t k);

/* Pivot selector function that selects a pivot based on the current array bounds*/ 
uint32_t selectPivot(int ip, int jp, uint32_t *a);

/* Calculates next mode based on the total amount of less than and pivot counts.
 * Relative_k is updated (passed by reference)
 */
enum mode decideNextMode(uint64_t less_than_total_count,uint64_t pivot_total_count,uint64_t *relative_k);

/* Simple initiation*/
INDICES initialIndices(int a_size);

/* Updating indices for each loop iteration to set the working part of the array based on the 
 * given mode.
 */
void updateIndices(INDICES *p, enum mode mode);

#endif
