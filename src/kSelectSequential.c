#include "kSelectSequential.h"
#include <stdio.h> // Used for error print statements
#include <stdlib.h> // Used for rand and srand
#include <time.h> // Used for time()
#include <stdbool.h> // Used for boolean type

// Array Partition.
RESULTS arrayPartition(uint32_t *a, uint32_t pivot, INDICES *p){
  RESULTS results;
  // Since jp and ip will be moving, the intial bounds of the array are stored.
  int working_partition_start=p->ip;
  int working_partition_end=p->jp;
  bool done=p->i>p->j; 
  uint32_t temp;
  // Main body of partition.
  while(!done){
    // Move i until you find a >p element
    while(p->i<=p->j && a[p->i]<=pivot){
      // If you find a pivot intance put it in back.
      if(a[p->i]==pivot){
        temp=a[p->i];
        a[p->i]=a[p->ip];
        a[p->ip++]=temp;
      }
      p->i++;
    }
    // Move j until you find a <p element
    while(p->i<=p->j && a[p->j]>=pivot){
      // If you find a pivot instance put it in front.
      if(a[p->j]==pivot){
        temp=a[p->j];
        a[p->j]=a[p->jp];
        a[p->jp--]=temp;
      }
      p->j--;
    }
    // If i and j have met, I'm done, no swapping needed.
    done=p->i>p->j;
    // NO SWAPPING IS NEEDED, the indices will move on their own in the next iteration.
    if(!done){
      temp=a[p->j];
      a[p->j]=a[p->i];
      a[p->i]=temp;
    }
  }
  // Calculate the return values 
  results.less_than_count=p->i-p->ip;
  results.pivot_count=p->ip-working_partition_start+working_partition_end-p->jp;
  return results;
}
// Main sequential function.
uint32_t kSelectSequential(uint32_t *a, int a_size, int k){
  // Error handling for parameters
  if(k>=a_size || k<0){
    printf("Input error: k must be within the array's bounds\n");
    exit(1);
  }
  // Initialize
  int relative_k=k; // This represents the desired k in the array that is currently worked.
  enum mode mode = LESS_THAN; // Doesn't matter at the beginning, decides the next partition to be worked.
  uint32_t pivot_val;
  int pivot_index;
  srand(time(NULL));
  INDICES p;
  p.i=p.ip=0;
  p.j=p.jp=a_size-1;
  RESULTS r;
  // Main body of function 
  do{
    // Depending on mode of operation, the values of p will need to be updated, to set the 
    // new search region.
    // This does nothing on initial call since ip==i and jp==j.
    if(mode==LESS_THAN){
      p.jp=p.j;
      p.i=p.ip;
    }
    else if(mode==MORE_THAN){
      p.ip=p.i;
      p.j=p.jp;
    }
    // IP and JP define the working region.
    pivot_index=(p.jp!=p.ip)?(p.ip+rand()%(p.jp-p.ip)):(p.ip);
    pivot_val=a[pivot_index];
    r=arrayPartition(a,pivot_val,&p);
    if(relative_k<=r.less_than_count-1){ // Look at <pivot part
      mode=LESS_THAN;
      // RELATIVE K STAYS THE SAME
    }
    else if(relative_k>=r.less_than_count+r.pivot_count){ // Look at >pivot part
      mode=MORE_THAN;
      relative_k=relative_k-r.less_than_count-r.pivot_count; // Relative k changes to still look for the right val. 
    }
    else{
      mode=STOP;
    }
  }while(mode!=STOP);
  return pivot_val;
}
