/* This tests the performance of kSelectParallel as a function of the number of array elements.
* The vectors are randomly generated ints. The algorithm is assumed to work from previous tests.
* The root stores the data in time_n_p.txt where p is the number of nodes used (4 cpus/node).
* Results are stored in the the folder specified by argv[1].
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <time.h>
#include <inttypes.h>
#include "arrayParsing.h"
#include "kSelectParallel.h"

#define SIZE_START 500000 // 500k 
#define SIZE_END 50000000 // 50mill
#define SIZE_STEP 500000 // 500k 
#define RUNS_PER_SIZE 30

double get_time(struct timeval a, struct timeval b);


int main(int argc,char** argv){
  if(argc!=2){
    printf("Usage: ./time_n_job [data location (no '/' at the end)]");
  }
  int world_rank,world_size;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);
  MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
  struct timeval a,b;
  struct timezone tz;
  uint32_t kSelectResult;
  uint64_t k;
  double avg_time;
  double time_samples[RUNS_PER_SIZE];
  ARRAY array;
  char* plot_location=(char*)malloc(100*sizeof(char));
  FILE* plot_file;
  srand(time(NULL));
  // Root responsible for data storing:
  if(world_rank==0){
    snprintf(plot_location,99,"%s/time_n_%d.txt",argv[1],world_size);
    plot_file=fopen(plot_location, "w");
  }
  // For each size 
  for(uint64_t size=SIZE_START;size<SIZE_END;size+=SIZE_STEP){
    // Determine the local size of each process:
    array.total_size=size;
    uint64_t start=world_rank*(array.total_size/world_size);
    uint64_t end=(world_rank==world_size-1)?(array.total_size-1):((world_rank+1)*(array.total_size/world_size));
    array.local_size=end-start+1;
    array.values=(uint32_t*)malloc(array.local_size*sizeof(uint32_t));
    // Repeat the algorithm RUNS_PER_SIZE times 
    for(int run_count=0;run_count<RUNS_PER_SIZE;run_count++){
      // Generate your local part.
      for(int i=0;i<array.local_size;i++){
        array.values[i]=rand(); 
      }
      // Root chooses k and BCasts it to everyone.
      if(world_rank==0){
        k=rand()%size;
      }
      MPI_Bcast(&k, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
      // Algorithm is run and time is stored in samples.
      gettimeofday(&a,&tz);
      kSelectParallel(array, k);
      gettimeofday(&b,&tz);
      time_samples[run_count]=get_time(a,b);
    }
    free(array.values);
    // Root gets avg and stores it in file 
    if(world_rank==0){
      avg_time=0;
      for(int run_count=0;run_count<RUNS_PER_SIZE;run_count++){
        avg_time+=time_samples[run_count];
      }
      avg_time/=RUNS_PER_SIZE;
      // Store in file 
      fprintf(plot_file,"%" PRIu64 " %f\n",size,avg_time);
    }
  }
  if(world_rank==0){
    printf("Job done\n");
    fclose(plot_file);
  }
  free(plot_location);
  return 0;
}

double get_time(struct timeval a, struct timeval b) {
  return (double)((b.tv_sec - a.tv_sec) * 1000000 + b.tv_usec - a.tv_usec) /
         1000000;
}
