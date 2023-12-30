#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include <stdbool.h>
#include "kSelectParallel.h" 
#include "kSelectSequential.h"
#include "arrayParsing.h"

uint32_t kSelectParallel(char *array_filename, uint64_t k){
  //MPI_Init(NULL,NULL);

  // Get basic MPI info of current node.
  int world_rank,world_size;
  MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);
  // Get your part of the array.
  ARRAY array=sharedFileParsing(array_filename,world_rank,world_size);
  if(k>=array.total_size){
    printf("k must be within array's bounds. Aborting..\n");
    exit(1);
  }
  srand(time(NULL));
  uint32_t pivot_val;
  uint64_t relative_k=k;
  uint64_t pivot_total_count,less_than_total_count;
  uint64_t temp_pivot_count,temp_less_than_count;
  int pivot_selector;
  bool pivot_is_available;
  // Bound setting variables.
  enum mode mode=LESS_THAN;
  INDICES p=initialIndices(array.local_size);
  RESULTS r;

  // The selector of the cluster is changed on each iteration to make the pivot selections more random.
  for(pivot_selector=0;mode!=STOP;pivot_selector=(pivot_selector+1)%world_size){
    // Pivot selection process.
    if(world_rank==pivot_selector){ // Current pivot selector.
      if(p.ip<=p.jp){ // If not empty..
        // Select a pivot, let the others know you have it and give it to them.
        pivot_is_available=true;
        pivot_val=selectPivot(p.ip,p.jp,array.values);
        MPI_Bcast(&pivot_is_available,1,MPI_C_BOOL,pivot_selector,MPI_COMM_WORLD);
        MPI_Bcast(&pivot_val,1,MPI_UINT32_T,pivot_selector,MPI_COMM_WORLD);
      } 
      else{ // If empty..
        // Let them know and skip to next pivot selector.
        pivot_is_available=false;
        MPI_Bcast(&pivot_is_available,1,MPI_C_BOOL,pivot_selector,MPI_COMM_WORLD);
        continue;
      }
    }
    else{ // Not the pivot selector.
      MPI_Bcast(&pivot_is_available,1,MPI_C_BOOL,pivot_selector,MPI_COMM_WORLD);
      if(pivot_is_available){ // If the ok is given, take the pivot and proceed.
        MPI_Bcast(&pivot_val,1,MPI_UINT32_T,pivot_selector,MPI_COMM_WORLD);
      }
      else{ // Else just skip to the next pivot selector.
        continue;
      }
    }
  
    // Partition given the pivot.
    r=arrayPartition(array.values,pivot_val,&p);
    // Pass results to 64 bit ints to avoid overflow from reduce.
    temp_pivot_count=(uint64_t)r.pivot_count;
    temp_less_than_count=(uint64_t)r.less_than_count;
    // Pass results to the Master (0).
    MPI_Reduce(&temp_less_than_count,&less_than_total_count,1,MPI_UINT64_T, MPI_SUM,0,MPI_COMM_WORLD);
    MPI_Reduce(&temp_pivot_count,&pivot_total_count,1,MPI_UINT64_T,MPI_SUM,0,MPI_COMM_WORLD);

    // Mode selection and preparation for the next loop iteration.
    if(world_rank==0){
      mode=decideNextMode(less_than_total_count,pivot_total_count,&relative_k);
    } // I am master.
    MPI_Bcast(&mode, 1, MPI_INT, 0, MPI_COMM_WORLD); // Everyone gets the next mode.
    if(mode!=STOP){ // If the procedure continues you update the indices for the next one.
      updateIndices(&p,mode);
    }
  }  

  //MPI_Finalize();
  free(array.values);
  return pivot_val;
}

void updateIndices(INDICES *p, enum mode mode){
    if(mode==LESS_THAN){
      p->jp=p->j;
      p->i=p->ip;
    }
    else if(mode==MORE_THAN){
      p->ip=p->i;
      p->j=p->jp;
    }
}

uint32_t selectPivot(int ip,int jp,uint32_t *a){
  int index=(ip!=jp)?(ip+rand()%(jp-ip)):(ip);
  return a[index];
}

enum mode decideNextMode(uint64_t less_than_total_count,uint64_t pivot_total_count,uint64_t *relative_k){
  enum mode mode;
  if(less_than_total_count>0&&*relative_k<=less_than_total_count-1){
    mode=LESS_THAN;
  }
  else if(*relative_k>=less_than_total_count+pivot_total_count){
    *relative_k=*relative_k-less_than_total_count-pivot_total_count;
    mode=MORE_THAN;
  }
  else{
    mode=STOP;
  }
  return mode;
}

INDICES initialIndices(int a_size){
  INDICES p;
  p.i=p.ip=0;
  p.j=p.jp=a_size-1;
  return p;
}
