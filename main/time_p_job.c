/* Used for the plot of time(p) given a single array file.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <time.h>
#include "arrayParsing.h"
#include "kSelectParallel.h"

#define RUNS_PER_FILE 30

double get_time(struct timeval a, struct timeval b);

int main(int argc,char** argv){
  if(argc!=3){
    printf("Usage: ./time_p_job [dataLocation] [fileName]\n");
    exit(1);
  }
  int world_rank,world_size;
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);
  MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
  struct timeval a,b;
  struct timezone tz;
  uint32_t kSelectResult;
  uint64_t k;
  double avg_time;
  double time_samples[RUNS_PER_FILE];
  ARRAY original,disposable; // For the sake of performance the array is stored twice.
  char* plot_location;
  FILE* plot_file;
  srand(time(NULL));
  // Root prints to file 
  if(world_rank==0){
    plot_location=(char*)malloc(100*sizeof(char));
    snprintf(plot_location,99,"%s/time_p_%d.txt",argv[1],world_size);
    plot_file=fopen(plot_location,"w");
  }
  // Get specified file from disk.
  original=sharedFileBinaryParsing(argv[2], world_rank, world_size);
  // Prepare disposable 
  disposable.local_size=original.local_size;
  disposable.total_size=original.total_size;
  disposable.values=(uint32_t*)malloc(disposable.local_size*sizeof(uint32_t));
  // Run the algorithm RUNS_PER_FILE times 
  for(int run_count=0;run_count<RUNS_PER_FILE;run_count++){
    // Hard copy original to disposable 
    for(int i=0;i<disposable.local_size;i++){
      disposable.values[i]=original.values[i];
    }
    // Root gets 64bit unsigned integer and BCasts it 
    if(world_rank==0){
      k=(((uint64_t)rand()<<32)|rand())%disposable.total_size;
    }
    MPI_Bcast(&k, 1, MPI_UINT64_T, 0,MPI_COMM_WORLD);
    // Run algorithm and store time 
    gettimeofday(&a,&tz);
    kSelectResult=kSelectParallel(disposable,k);
    gettimeofday(&b,&tz);
    if(world_rank==0){
      time_samples[run_count]=get_time(a,b);
    }  
  }
  if(world_rank==0){
    avg_time=0;
    for(int run_count=0;run_count<RUNS_PER_FILE;run_count++){
      avg_time+=time_samples[run_count];
    }
    avg_time/=RUNS_PER_FILE;
    fprintf(plot_file,"%s %f\n",argv[2],avg_time);
  }
  free(original.values);
  free(disposable.values);
  if(world_rank==0){
    free(plot_location);
    fclose(plot_file);
  }
  if(world_rank==0){
    printf("Job done\n");
  }
  MPI_Finalize();
  return 0;
}

double get_time(struct timeval a, struct timeval b) {
  return (double)((b.tv_sec - a.tv_sec) * 1000000 + b.tv_usec - a.tv_usec) /
         1000000;
}
