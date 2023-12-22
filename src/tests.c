#include "tests.h"
#include "kSelectParallel.h"
#include "kSelectSequential.h"
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Classic insertion sort.
void insertion_sort(int *arr, int n)
{
    int i, key, j;
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

int testSeqSelect(int a_max_size, int a_max_val){
  int fails=0;
  int *a;
  int kResult;

  srand(time(NULL));
  for(int size=1;size<=a_max_size;size++){
    for(int k=0;k<size;k++){
      // Create array 
      a=(int*)malloc(size*sizeof(int));
      for(int i=0;i<size;i++){
        a[i]=rand()%a_max_val;
      }
      kResult=kSelectSequential(a,size,k);      
      insertion_sort(a,size);

      if(kResult!=a[k]){
        fails++;
        printf("Wrong result with size: %d and k:%d\n",size,k);
        printf("Expected:%d Got:%d\n",a[k],kResult);
        getchar();
      }
      free(a);
    }
  }
  return fails;
}

int testParallelSelect(int a_max_size, int a_max_val){
  int fails=0;
  int count=0;
  int kResult;
  int *a;
  int world_rank;
  FILE* temp;
  MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
  srand(time(NULL));
  for(int size=1;size<=a_max_size;size++){
    for(int k=0;k<size;k++){
      // Only root does the creation
      if(world_rank==0){ // All of this is sequential.
        printf("Test %d of %d\n",count,(a_max_size+1)*a_max_size/2);
        // Create array 
        a=(int*)malloc(size*sizeof(int));
        for(int i=0;i<size;i++){
          a[i]=rand()%a_max_val;
        }
        // Write it to temp file
        if((temp=fopen("temp.txt","w"))==NULL){
          printf("Error in opening test.txt file\n");
        }
        fprintf(temp,"%d\n\n",size);
        for(int i=0;i<size;i++){
          fprintf(temp,"%d\n",a[i]);
        }
        fclose(temp);
        // Insertion sort for the result
        insertion_sort(a, size);
      }
      MPI_Barrier(MPI_COMM_WORLD);
      kResult=kSelectParallel("temp.txt", k);
      MPI_Barrier(MPI_COMM_WORLD);
      if(world_rank==0){
        if(kResult!=a[k]){
          fails++;
          printf("Wrong result with size: %d and k: %d\n",size,k);
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
