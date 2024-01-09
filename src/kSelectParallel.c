#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include <stdbool.h>
#include "kSelectParallel.h" 
#include "kSelectSequential.h"
#include "arrayParsing.h"

uint32_t kSelectParallel(const char *array_filename, uint64_t k,bool filename_is_URL){
  //MPI_Init(NULL,NULL);

  // Get basic MPI info of current node.
  int world_rank,world_size;
  MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);
  // Get your part of the array.
  ARRAY array;
  if(filename_is_URL){
    array=getURLFile(array_filename, world_rank, world_size, WIKI_MAX_PER_CURL);
  }
  else{
    array=sharedFileParsing(array_filename,world_rank,world_size);
  }
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

uint32_t kSelectParallel2(const char *array_filename, uint64_t k,bool filename_is_URL){
  //MPI_Init(NULL,NULL);

  // Get basic MPI info of current node.
  int world_rank,world_size;
  MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);
  // Get your part of the array.
  ARRAY array;
  if(filename_is_URL){
    array=getURLFile(array_filename, world_rank, world_size, WIKI_MAX_PER_CURL);
  }
  else{
    array=sharedFileParsing(array_filename,world_rank,world_size);
  }
  if(k>=array.total_size){
    printf("k must be within array's bounds. Aborting..\n");
    exit(1);
  }
  srand(time(NULL));
  uint32_t pivot_val;
  uint64_t relative_k=k;
  uint64_t pivot_total_count,less_than_total_count;
  uint64_t temp_pivot_count,temp_less_than_count;
  uint64_t working_total_size=array.total_size;
  int pivot_selector;
  bool pivot_is_available;
  // Bound setting variables.
  enum mode mode=LESS_THAN;
  INDICES p=initialIndices(array.local_size);
  RESULTS r;

  // The selector of the cluster is changed on each iteration to make the pivot selections more random.
  for(pivot_selector=0;mode!=STOP;pivot_selector=(pivot_selector+1)%world_size){
    // Exclude possibility of total gathering.
    if(mode==GATHER_LT || mode==GATHER_MT){
      return gatherToRootAndFinish(array.values, world_rank, world_size, p, relative_k);
    }
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
      if(mode==LESS_THAN){
        working_total_size=less_than_total_count;
        mode=(working_total_size<array.local_size)?(GATHER_LT):(mode);
      }
      else if(mode==MORE_THAN){
        working_total_size=working_total_size-less_than_total_count-pivot_total_count;
        mode=(working_total_size<array.local_size)?(GATHER_MT):(mode);
      }
    } // I am master.
    MPI_Bcast(&mode, 1, MPI_INT, 0, MPI_COMM_WORLD); // Everyone gets the next mode.
    if(mode!=STOP){ // If the procedure continues you update the indices for the next one.
      updateIndices(&p,mode);
    }
  }  
  //MPI_Finalize();
  free(array.values);
  if(world_rank==0)
    return pivot_val;
  else
    return 0;
}

// It is assumed that the indices have already been set up before the procedure.
uint32_t gatherToRootAndFinish(uint32_t *array,int world_rank, int world_size, INDICES indices,uint64_t k){
  int *sizes=NULL;
  int *displacements=NULL;
  if(world_rank==0){
    sizes=(int*)malloc(world_size*sizeof(int));
    displacements=(int*)malloc((world_size+1)*sizeof(int));
  }
  int array_size=indices.j-indices.i+1;
  // Root gets individual size from all processes. (rcvcounts)
  MPI_Gather(&array_size, 1, MPI_INT, sizes, 1, MPI_INT, 0, MPI_COMM_WORLD);
  // Root moves all of its working elements to the start of its array.
  if(world_rank==0){
    int count=0;
    for(int i=indices.ip;i<=indices.jp;i++){
      array[count++]=array[i]; // Moving data from lower index to higher for no data loss.
    }
    // Root creates displ array.
    displacements[0]=0;
    for(int i=1;i<=world_size;i++){
      displacements[i]=displacements[i-1]+sizes[i-1];
    }
    indices.i=indices.ip=0;
    indices.j=indices.jp=array_size-1;
  }
  // Everything is gathered.
  MPI_Gatherv(&array[indices.ip], array_size, MPI_UINT32_T, array, sizes, displacements, MPI_UINT32_T, 0, MPI_COMM_WORLD);
  // kSelectSequential is executed.
  if(world_rank==0){
    uint32_t result=kSelectSequential(array, displacements[world_size], k);
    return result;
  }
  else{
    return 0;
  }
  // Slaves return 0, root returns result.
}

void updateIndices(INDICES *p, enum mode mode){
    if(mode==LESS_THAN || mode==GATHER_LT){
      p->jp=p->j;
      p->i=p->ip;
    }
    else if(mode==MORE_THAN || mode==GATHER_MT){
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
