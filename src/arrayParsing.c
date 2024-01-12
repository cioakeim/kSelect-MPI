#include "arrayParsing.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/system.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <mpi.h>

// Local file parsing (in txt form)
// Slow way if the file is large, size all processes reads the whole file.
ARRAY sharedFileParsing(const char *file_name, int slot_id, int slot_count){
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
    if(count<0){
      printf("Error! Int overflow. Array node size is too large for an int type. Aborting..");
      exit(1);
    }
    fscanf(array_file,"%" PRIu32 " ",&a.values[count++]);
    read_cnt++;
    for(int i=0;i<(slot_count-1) && fscanf(array_file,"%" PRIu32 " ",&dont_use_this_value)!=EOF;i++,read_cnt++);
  }
  fclose(array_file);
  a.local_size=count;
  return a;
}

// The file is split as evenly as possible.
ARRAY sharedFileBinaryParsing(const char* file_name, int world_rank,int world_size){
  ARRAY result;
  struct stat file_status;
  FILE* array_file;
  if(stat(file_name,&file_status)<0){
    printf("Error in file size retrieval\n");
    exit(1);
  }
  result.total_size=file_status.st_size/4; // Truncate extra bytes that don't form an uint32_t
  // Calculate byte range 
  long int start_byte=4*world_rank*(result.total_size/world_size);
  long int end_byte=(world_rank==world_size-1)?(4*result.total_size-1):(4*(world_rank+1)*(result.total_size/world_size)-1);
  result.local_size=(end_byte-start_byte+1)/4;
  // Open file 
  array_file=fopen(file_name,"rb+");
  if(!array_file){
    printf("sharedFileBinaryParsing error. Couldn't open file. Aborting\n");
    exit(1);
  }
  fseek(array_file, start_byte, SEEK_SET);
  // Get the part that you need 
  result.values=(uint32_t*)malloc(result.local_size*sizeof(uint32_t));
  size_t test;
  if((test=fread((void*)result.values, sizeof(uint32_t), result.local_size, array_file))!=result.local_size){
    printf("Error in data read...\n");
    printf("Rank %d expected %d got %zu",world_rank,result.local_size,test);
    exit(1);
  }
  return result;
}

// URL Parsing functions.

// Write callback function for curl.
size_t write_callback(void* data, size_t size, size_t nmemb, void* dest){
  STREAM* stream=(STREAM*)dest;
  size_t realsize=size*nmemb;
  memcpy(&(stream->data[stream->current_byte_size]),data,realsize);
  stream->current_byte_size+=realsize;
  return realsize;
}

// Function that uses curl to actually get the data 
// Total size is in ints (not in bytes).
ARRAY getURLPartition(const char *url,int world_rank,int world_size,uint64_t total_size){
  // Input check.
  if(world_rank<0||world_rank>=world_size){
    printf("GetURL: World rank out of bounds. Exiting\n");
    exit(1);
  }
  CURL* curl_handle;
  CURLcode res; // Error checking.
  ARRAY result;
  STREAM stream;
  // Libcurl init.
  curl_handle=curl_easy_init();
  if(!curl_handle){
    printf("Error in easy_init\n");
    exit(1);
  }
  // Init Result 
  result.total_size=total_size;
  // Calculate range from data given.
  uint64_t start_byte=4*world_rank*(result.total_size/world_size);
  uint64_t end_byte=(world_rank==world_size-1)?(4*result.total_size-1):(4*(world_rank+1)*(result.total_size/world_size)-1);
  // Save range to string.
  char* range=(char*)malloc(100*sizeof(char));
  sprintf(range,"%" PRIu64 "-%" PRIu64 "",start_byte,end_byte);
  // Init stream.
  stream.total_byte_size=end_byte-start_byte+1;
  stream.data=(char*)malloc(stream.total_byte_size*sizeof(char));
  stream.current_byte_size=0;
  // Setup request.
  curl_easy_setopt(curl_handle, CURLOPT_URL,url);
  curl_easy_setopt(curl_handle, CURLOPT_RANGE,range);
  curl_easy_setopt(curl_handle,CURLOPT_WRITEDATA,(void*)&stream.data);
  curl_easy_setopt(curl_handle,CURLOPT_WRITEFUNCTION,write_callback);
  // Perform.
  res=curl_easy_perform(curl_handle);
  if(res!=CURLE_OK){
    printf("Error on data gathering\n");
    exit(1);
  }
  if(stream.current_byte_size!=stream.total_byte_size){
    printf("Missing bytes: %" PRIu64 " of %" PRIu64 "\n",stream.total_byte_size-stream.current_byte_size,stream.total_byte_size);
  }
  // Cleanup curl.
  curl_easy_cleanup(curl_handle);
  // Set up result.
  result.local_size=stream.current_byte_size/4;
  result.values=(uint32_t*)stream.data;
  return result;
}

// Returns size of file in INTS.
uint64_t getURLSize(const char *url){
  CURL* curl_handle;
  uint64_t file_size;
  curl_off_t dl;
  curl_handle=curl_easy_init();
  if(!curl_handle){
    printf("Error in easy_init\n");
    exit(1);
  }
  curl_easy_setopt(curl_handle, CURLOPT_URL,url);
  curl_easy_setopt(curl_handle,CURLOPT_NOBODY,1);
  curl_easy_perform(curl_handle);
  curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T,&dl);
  file_size=dl/4; // Extra bytes truncated.
  curl_easy_cleanup(curl_handle);
  return file_size;
}

// Main function of parsing the file.
ARRAY getURLFile(const char *url, int world_rank, int world_size, int maxThreadsPerCurl){
  ARRAY result;
  uint64_t total_size; // IN INTS.
  curl_global_init(CURL_GLOBAL_ALL);
  // Root gets size.
  if(world_rank==0){
    total_size=getURLSize(url);
  }
  MPI_Bcast(&total_size, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
  // Only few at a time get file in parallel.
  for(int start_rank=0;start_rank<world_size;start_rank+=maxThreadsPerCurl){
    if(world_rank>=start_rank && world_rank<start_rank+maxThreadsPerCurl){
      result=getURLPartition(url, world_rank, world_size, total_size);
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
  curl_global_cleanup();
  return result;
}

// Function that scans only a portion of the file to get a fixed size array.
// Same as getURLFile without the size retrieval.
ARRAY getURLFixedSize(const char *url,int world_rank,int world_size,int maxThreadsPerCurl,uint64_t total_size){
  ARRAY result;
  curl_global_init(CURL_GLOBAL_ALL);
  // Only few at a time get file in parallel.
  for(int start_rank=0;start_rank<world_size;start_rank+=maxThreadsPerCurl){
    if(world_rank>=start_rank && world_rank<start_rank+maxThreadsPerCurl){
      result=getURLPartition(url, world_rank, world_size, total_size);
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
  curl_global_cleanup();
  return result;
}
