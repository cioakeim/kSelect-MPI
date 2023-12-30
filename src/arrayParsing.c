#include "arrayParsing.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

ARRAY sharedFileParsing(char *file_name, int slot_id, int slot_count){
  ARRAY a;
  FILE *array_file;
  uint64_t file_array_size;

  array_file=fopen(file_name,"r");
  if(array_file==NULL){
    printf("Error in reading array file... Aborting.\n");
    exit(1);
  }
  if(fscanf(array_file,"%" PRIu64 " ",&file_array_size)==EOF){
    printf("Error! This is process %d.Is the array file empty?\n",slot_id);
    exit(1);
  }
  a.local_size=0;
  a.total_size=file_array_size;
  a.values=(uint32_t*)malloc(((file_array_size/slot_count)+1)*sizeof(uint32_t));
  int count=0;
  int read_cnt=0; // Counts number of ELEMENTS read.
  uint32_t dont_use_this_value;
  // Reading is done in cycles. Each slot reads with step of slot_count.
  for(int i=0;i<slot_id && fscanf(array_file,"%" PRIu32 " \n",&dont_use_this_value)!=EOF;i++,read_cnt++);

  while(!feof(array_file)&&(count<(file_array_size/slot_count)+1)&&(read_cnt<file_array_size)){
    fscanf(array_file,"%" PRIu32 " ",&a.values[count++]);
    read_cnt++;
    for(int i=0;i<(slot_count-1) && fscanf(array_file,"%" PRIu32 " ",&dont_use_this_value)!=EOF;i++,read_cnt++);
  }
  fclose(array_file);
  a.local_size=count;
  return a;
}
