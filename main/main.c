/* Main function that uses the kSelect algorithm
* The file in this case is loaded from the disk and is read in binary.
* Only one result is returned.
*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "kSelectParallel.h"
#include "mpi.h"
#include "arrayParsing.h"
 

int main(int argc, char**argv){
  if(argc!=3){
    printf("Correct usage: ./main [fileName] [k]");
  }
  MPI_Init(&argc,&argv);
  // Get basic MPI info of current node.
  int world_rank,world_size;
  MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);
  char *ptr;
  uint64_t k=strtoul(argv[1],&ptr,10);
  ARRAY array=sharedFileBinaryParsing(argv[1], world_rank, world_size);
  uint32_t result=kSelectParallel(array, k);
  if(world_rank==0){
    printf("Job done. Result is: %" PRIu32 "\n",result);
  }
  MPI_Finalize();
  return 0;
}
