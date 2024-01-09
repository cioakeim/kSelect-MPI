#include "tests.h"
#include "kSelectParallel.h"
#include "kSelectSequential.h"
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>

// Classic insertion sort.
void insertion_sort(uint32_t *arr, int n)
{
    int i, j;
    uint32_t key;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

int testSeqSelect(int a_max_size, uint32_t a_max_val){
  int fails=0;
  uint32_t *a;
  uint32_t kResult;

  srand(time(NULL));
  for(int size=1;size<=a_max_size;size++){
    for(int k=0;k<size;k++){
      // Create array 
      a=(uint32_t*)malloc(size*sizeof(uint32_t));
      for(int i=0;i<size;i++){
        a[i]=rand()%a_max_val;
      }
      kResult=kSelectSequential(a,size,k);      
      insertion_sort(a,size);

      if(kResult!=a[k]){
        fails++;
        printf("Wrong result with size: %d and k:%d\n",size,k);
        printf("Expected:%" PRIu32" Got:%" PRIu32 "\n",a[k],kResult);
        getchar();
      }
      free(a);
    }
  }
  return fails;
}

int testParallelSelect(uint64_t a_max_size, uint32_t a_max_val){
  uint64_t fails=0;
  uint64_t count=0;
  uint32_t kResult;
  uint32_t *a;
  int world_rank;
  FILE* temp;
  MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
  srand(time(NULL));
  for(uint64_t size=1;size<=a_max_size;size++){
    for(uint64_t k=0;k<size;k++){
      // Only root does the creation
      if(world_rank==0){ // All of this is sequential.
        printf("Test %" PRIu64 " of %" PRIu64 ", size: %" PRIu64 "\n",count,(a_max_size+1)*a_max_size/2,size);
        // Create array 
        a=(uint32_t*)malloc(size*sizeof(uint32_t));
        for(uint64_t i=0;i<size;i++){
          a[i]=rand()%a_max_val;
        }
        // Write it to temp file
        if((temp=fopen("temp.txt","w"))==NULL){
          printf("Error in opening test.txt file\n");
        }
        fprintf(temp,"%" PRIu64 "\n\n",size);
        for(uint64_t i=0;i<size;i++){
          fprintf(temp,"%" PRIu32 "\n",a[i]);
        }
        fclose(temp);
        // Insertion sort for the result
        insertion_sort(a, size);
      }

      MPI_Barrier(MPI_COMM_WORLD);
      kResult=kSelectParallel2("temp.txt", k,0);
      MPI_Barrier(MPI_COMM_WORLD);
      if(world_rank==0){
        if(kResult!=a[k]){
          fails++;
          printf("Wrong result with size: %" PRIu64 " and k: %" PRIu64 "\n",size,k);
          printf("Expected: %" PRIu32 " got %" PRIu32 "\n",a[k],kResult);
          printf("Near sorted point: %" PRIu32 " %" PRIu32 " %" PRIu32 "\n",a[k-1],a[k],a[k+1]);
          getchar();
        }
        // Cleanup this test.
        free(a);
        count++;
      }
    }
  }
  remove("temp.txt");
  return fails;
}
