/* kSelectSequential.h:
 * This module implements the procedures used for the k select algorithm, where the input is an array
 * a and the output is the value of the kth largest element of the array.
 *
 * These modules are also used in the parallel version of the algorithm using Open MPI.
*/
#ifndef K_SELECT_SEQUENTIAL_H
#define K_SELECT_SEQUENTIAL_H

#define A_SIZE 20
#define A_MAX 30
#define K 13

#include <stdint.h>

/* Modes enumerator for the paritition function: 
 * LESS_THAN: Use partition from ip to i-1.
 * MORE_THAN: Use partition from j+1 to jp.
 * STOP: Is used only with MPI to send to workers that the prodecure is done.
 * SKIP: Is used with MPI to skip a loop iteration if the current master is empty.
 * GATHER (LT/MT): Is used to collect all the working elements to the 0 rank for sequential solution.
 */
enum mode{LESS_THAN,MORE_THAN,STOP,SKIP,GATHER_LT,GATHER_MT};

/* Struct used to return the results of the partition.*/ 
typedef struct{
  int less_than_count; // Number of elements in scanned partition that are <pivot.
  int pivot_count; // Number of elements in scanned partition that are ==pivot.
} RESULTS;

/* Since using a static type is not possible because the array size isn't known, 
 * a struct is used to clean up the argument passing of the indices, since all of them will 
 * be passed by reference, as they need to be changed.
 */ 
typedef struct{
  int i;
  int ip;
  int j;
  int jp;
} INDICES;


/* This function is used for the array partition according to the pivot specified and the 
 * mode of operation. The mode is used to specify which part of the array is still used 
 * since the function will be called multiple times.
 * Input:
 *  a (int*): the array that is partitioned. 
 *  pivot (int): the value according to which the partitioning will be done.
 *  p (INDICES*): the 4 main indices of the partition prodecure.
 * Output: 
 *  RESULTS: Used to decide if k was found or the next mode of operation.
 *
 *  a's form after the procedure:
 *    a(a_start:ip-1)==p && a(jp+1:a_end-1)==p && a(ip:i-1)<p && a(j+1:jp)>p.
*/
RESULTS arrayPartition(uint32_t *a, uint32_t pivot, INDICES *p);

/* Main function of the module implemented sequentially. Returns the value of kth largest element 
 * in the array. 
 * Inputs:
 *  a (*int): Array.
 *  a_size (int): Size of array.
 *  k (int): Desired position of the array.
 * Output: 
 *  Value of the kth largest element of a.
*/
uint32_t kSelectSequential(uint32_t *a, int a_size, int k);

#endif
