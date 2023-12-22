/* Main function that tests the kSelect algorithm*/ 
#include <stdio.h>
#include <stdlib.h>
#include "kSelectParallel.h"
#include "mpi.h"
#include "tests.h"
#include "arrayParsing.h"
 

int main(int argc, char**argv){
  int world_rank;
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);

  int fails=testParallelSelect(1000, 500);
  if(world_rank==0){
    printf("Fails: %d\n",fails);
  }
  MPI_Finalize();
  return 0;
}
