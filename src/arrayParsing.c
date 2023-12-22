#include "arrayParsing.h"
#include <stdlib.h>
#include <stdio.h>

ARRAY sharedFileParsing(char *file_name, int slot_id, int slot_count){
  ARRAY a;
  FILE *array_file;
  int file_array_size;

  array_file=fopen(file_name,"r");
  if(array_file==NULL){
    printf("Error in reading array file... Aborting.\n");
    exit(1);
  }
  if(fscanf(array_file,"%d ",&file_array_size)==EOF){
    printf("Error! This is process %d.Is the array file empty?\n",slot_id);
    exit(1);
  }
  a.local_size=0;
  a.total_size=file_array_size;
  a.values=(int*)malloc(((file_array_size/slot_count)+1)*sizeof(int));
  int count=0;
  int dont_use_this_value;
  int read_cnt=0; // Counts number of ELEMENTS read.
  // Reading is done in cycles. Each slot reads with step of slot_count.
  for(int i=0;i<slot_id && fscanf(array_file,"%d \n",&dont_use_this_value)!=EOF;i++,read_cnt++);

  while(!feof(array_file)&&(count<(file_array_size/slot_count)+1)&&(read_cnt<file_array_size)){
    fscanf(array_file,"%d ",&a.values[count++]);
    read_cnt++;
    for(int i=0;i<(slot_count-1) && fscanf(array_file,"%d",&dont_use_this_value)!=EOF;i++,read_cnt++);
  }
  fclose(array_file);
  a.local_size=count;
  return a;
}
