/* Given the arguments [file] [p] finds and stores all results of kParallelSelect when 
* k = size*i/p, (i from 0 to p-1).
* Each process stores 2 copies of its array portion to avoid the slowdown of reading multiple times form
* the disk, but it's memory inefficient.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <time.h>
#include <inttypes.h>
#include "arrayParsing.h"
#include "kSelectParallel.h"

int main(int argc,char **argv){
  if(argc!=4){
    printf("Usage: ./find_even_percentiles [fileLocation] [numOfResults] [storeLocation]\n");
    exit(1);
  }
  int world_rank,world_size;
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);
  MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
  uint32_t kSelectResult;
  uint64_t k;
  ARRAY original,disposable;
  char* result_location;
  FILE* result_file;
  srand(time(NULL));
  // Root prints to file 
  if(world_rank==0){
    result_location=(char*)malloc(100*sizeof(char));
    snprintf(result_location,99,"%s/percentile_result_%d.txt",argv[3],world_size);
    if(world_rank==0){
      printf("Saving to %s\n",result_location);
    }
    result_file=fopen(result_location,"w");
    if(!result_file){
      printf("Error in result file opening\n");
      exit(1);
    }
  }
  char* ptr;
  uint64_t p=strtoul(argv[2],&ptr, 10);
  // Get specified file from disk.
  original=sharedFileBinaryParsing(argv[1], world_rank, world_size);
  // Prepare disposable 
  disposable.local_size=original.local_size;
  disposable.total_size=original.total_size;
  disposable.values=(uint32_t*)malloc(disposable.local_size*sizeof(uint32_t));
  for(int current_result=0;current_result<p;current_result++){
    // Hard copy original to disposable 
    for(int i=0;i<disposable.local_size;i++){
      disposable.values[i]=original.values[i];
    }
    // Get k.
    k=current_result*(disposable.total_size/p);
    MPI_Barrier(MPI_COMM_WORLD);
    kSelectResult=kSelectParallel(disposable, k);
    // Root stores result. 
    if(world_rank==0){
      fprintf(result_file,"%" PRIu64 " %" PRIu32 "\n",k,kSelectResult);
    }
  }
  if(world_rank==0){
    fclose(result_file);
    printf("Job done\n");
  }
  free(disposable.values);
  free(original.values);
  free(result_location);
  MPI_Finalize();
  return 0;
}
