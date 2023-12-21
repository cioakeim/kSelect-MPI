#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "kSelectParallel.h" 
#include "kSelectSequential.h"
#include "arrayParsing.h"


int kSelectParallel(char *array_filename, int k){
  int result=0;
  MPI_Init(NULL,NULL);

  int world_rank,world_size;
  MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);

  ARRAY array=sharedFileParsing(array_filename,world_rank,world_size);

  if(k<0||k>=array.total_size){
    printf("k must be within array's bounds. Aborting..\n");
    exit(1);
  }
  srand(time(NULL));
  int pivot_val,relative_k=k;
  int pivot_total_count,less_than_total_count;
  int next_master=0,current_master=0;
  enum mode mode=LESS_THAN;
  INDICES p=initialIndices(array.local_size);
  RESULTS r;

  do{
    // Set boundaries 
    if(mode==LESS_THAN){
      p.jp=p.j;
      p.i=p.ip;
    }
    else if(mode==MORE_THAN){
      p.ip=p.i;
      p.j=p.jp;
    }
    if(world_rank==current_master)
      pivot_val=selectPivot(p.ip,p.jp,array.values); 

    // This master is empty, choose the next one
    if(world_rank==current_master && p.ip==p.jp){
      next_master=(current_master+1)%world_size;
    }
    MPI_Bcast(&next_master,1,MPI_INT,current_master,MPI_COMM_WORLD);
    // Send relative_k to next master 
    if(current_master!=next_master && world_rank==current_master){
      MPI_Send(&relative_k, 1, MPI_INT, next_master, 0, MPI_COMM_WORLD);
    }
    else if(current_master!=next_master&&world_rank==next_master){
      MPI_Recv(&relative_k,1,MPI_INT,current_master,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }
    current_master=next_master;
    MPI_Bcast(&pivot_val,1,MPI_INT,current_master,MPI_COMM_WORLD);
    printf("BCast of pivot done, val:%d,\nrelative k:%d\n",pivot_val,relative_k);
    r=arrayPartition(array.values,pivot_val,&p);

    MPI_Reduce(&r.less_than_count,&less_than_total_count,1,MPI_INT, MPI_SUM,current_master,MPI_COMM_WORLD);
    MPI_Reduce(&r.pivot_count,&pivot_total_count,1,MPI_INT,MPI_SUM,current_master,MPI_COMM_WORLD);
    printf("Reduce results:%d %d,relative k:%d\n",less_than_total_count,pivot_total_count,relative_k);
    if(world_rank==current_master)
      mode=decideNextMode(less_than_total_count,pivot_total_count,&relative_k);
    printf("Next mode: %d\n",mode);
    getchar();
    MPI_Bcast(&mode, 1, MPI_INT, 0, MPI_COMM_WORLD);
  }while(mode!=STOP);
  printf("Stopped, result is: %d\n",result);

  // Return result to 0 (calling process) 
  if(world_rank==current_master){
    MPI_Send(&pivot_val,1,MPI_INT,0,1,MPI_COMM_WORLD);
  }
  if(world_rank==0){
    MPI_Recv(&pivot_val, 1, MPI_INT, current_master, 1, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
  }
  MPI_Finalize();
  free(array.values);
  return pivot_val;
}



int kSelectMaster(int *a, int a_size, int k, int world_size){
  // Master Specific.
  int relative_k=k;
  srand(time(NULL));
  int pivot_val,pivot_index;
  int less_than_total_count,pivot_total_count;
  // For all processes.
  enum mode mode=LESS_THAN;
  INDICES p;
  p.i=p.ip=0;
  p.j=p.jp=a_size-1;
  RESULTS r;

  do{
    // Set boundaries of the next partition.
    if(mode==LESS_THAN){
      p.jp=p.j;
      p.i=p.ip;
    }
    else if(mode==MORE_THAN){
      p.ip=p.i;
      p.j=p.jp;
    }
    less_than_total_count=pivot_total_count=0;
    pivot_index=(p.jp!=p.ip)?(p.ip+rand()%(p.jp-p.ip)):(p.ip);
    pivot_val=a[pivot_index];
    MPI_Bcast(&pivot_val,1,MPI_INT,0,MPI_COMM_WORLD);
    printf("BCast of pivot done, val:%d,\nrelative k:%d\n",pivot_val,relative_k);
    r=arrayPartition(a,pivot_val,&p);
    printf("Slave results: %d %d\n",r.less_than_count,r.pivot_count);

    // Same decision code with sequential but with bCast and Reduce added
    MPI_Reduce(&r.less_than_count,&less_than_total_count,1,MPI_INT, MPI_SUM,0,MPI_COMM_WORLD);
    MPI_Reduce(&r.pivot_count,&pivot_total_count,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    printf("Reduce results:%d %d\n",less_than_total_count,pivot_total_count);
    getchar();
    MPI_Bcast(&mode, 1, MPI_INT, 0, MPI_COMM_WORLD);
  }while(mode!=STOP);
  return pivot_val;
}

void kSelectSlave(int *a, int a_size){
  INDICES p;
  p.i=p.ip=0;
  p.j=p.jp=a_size-1;
  RESULTS r;
  enum mode mode;
  int pivot_val;

  do{
    // Set boundaries of the next partition.
    if(mode==LESS_THAN){
      p.jp=p.j;
      p.i=p.ip;
    }
    else if(mode==MORE_THAN){
      p.ip=p.i;
      p.j=p.jp;
    }

    MPI_Bcast(&pivot_val,1,MPI_INT,0,MPI_COMM_WORLD);
    r=arrayPartition(a,pivot_val,&p);
    printf("Slave results: %d %d\n",r.less_than_count,r.pivot_count);

    MPI_Reduce(&r.less_than_count,NULL,1,MPI_INT, MPI_SUM,0,MPI_COMM_WORLD);
    MPI_Reduce(&r.pivot_count,NULL,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

    MPI_Bcast(&mode, 1, MPI_INT, 0, MPI_COMM_WORLD);
  }while(mode!=STOP);
  return;
}

int selectPivot(int ip,int jp,int *a){
  int index=(ip!=jp)?(ip+rand()%(jp-ip)):(ip);
  return a[index];
}

enum mode decideNextMode(int less_than_total_count,int pivot_total_count,int *relative_k){
  enum mode mode;
  if(*relative_k<=less_than_total_count-1){
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








