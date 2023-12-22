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

/* Master node prodecure.
*   The Master node collects the counts from the slave nodes and calculates the next mode of operation.
*   If the size of the array currently searches becomes small enough to fit to the Master Node, this 
*   node receives the elements of each slave and finishes the prodecure using the sequential algorithm.
*   Inputs:
*     a (int*): The array that the Master initially works on.
*     a_size (int): Size of a.
*     k (int): The desired position.
*     world_size (int): Total number of processes.
*   Returns: 
*     The value of the k-th largest element of the WHOLE array (not a).
*/
int kSelectMaster(int *a, int a_size,int k, int world_size);

/* Slave node prodecure.
*   The Slave nodes continuously partition the array 'a' using the mode of operation given by the 
*   Master node, until the mode is STOP. Then they return.
*   Inputs: 
*     a (int*): The array that the Slave works on.
*     a_size (int): Size of a.
*   Returns: 
*     Nothing, all I/O is done with MPI.
*/
void kSelectSlave(int *a, int a_size);
#endif
