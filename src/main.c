/* Main function that tests the kSelect algorithm*/ 
#include <stdio.h>
#include <stdlib.h>
#include "kSelectParallel.h"
#include "mpi.h"
#include "tests.h"
#include "arrayParsing.h"
 

int main(int argc, char**argv){
  MPI_Init(&argc,&argv);

  const char *url="https://dumps.wikimedia.org/other/static_html_dumps/current/el/wikipedia-el-html.tar.7z";

  uint64_t k=10;
  uint32_t value=kSelectParallel2(url,k,1);
  printf("Value is %d\n",value);


  MPI_Finalize();
  return 0;
}
