#include "tests.h"
#include "kSelectSequential.h"
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
