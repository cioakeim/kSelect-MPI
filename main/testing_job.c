/*  Script used for the testing job in the HPC server.
*   Multiple arrays of different sizes are created and kSelectParallel is used on all of them
*   for all sizes and all k until the specified limits.
*   This script is used in combination with parametric job batching to ensure that all the 
*   results are valid for different configurations of nodes
*/
#include "mpi.h"
#include "tests.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc,char **argv){
  if(argc!=2){
    printf("Error. Correct usage: ./testing_job [temp file location]\n");
    exit(1);
  }
  MPI_Init(NULL, NULL);
  int world_rank,world_size;
  MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);

  uint64_t a_max_size=10000;
  uint64_t a_max_val=7000;
  int fails=testParallelSelect(a_max_size, a_max_val,argv[1]);
  
  if(world_rank==0){
    printf("World size: %d, fails: %d\n",world_size,fails);
  }
  return 0;
}
