/* Main function that tests the kSelect algorithm*/ 
#include <stdio.h>
#include <stdlib.h>
#include "kSelectParallel.h"
#include "mpi.h"
#include "tests.h"
#include "arrayParsing.h"
 

int main(int argc, char**argv){
  MPI_Init(&argc,&argv);
  // Get basic MPI info of current node.
  int world_rank,world_size;
  MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);

  //const char *url="https://dumps.wikimedia.org/other/static_html_dumps/current/el/wikipedia-el-html.tar.7z";
  //ARRAY array=getURLFile(url, world_rank, world_size, WIKI_MAX_PER_CURL);

  //uint64_t k=10;
  //uint32_t value=kSelectParallel(array,k);
  //printf("Value is %d\n",value);


  int fails=testParallelSelect(50000, 5000);
  printf("Fails: %d\n",fails);

  MPI_Finalize();
  return 0;
}
