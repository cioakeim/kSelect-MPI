/*  
*  This module is used for the parsing of the array (that is stored in disk) to the nodes of the 
*  MPI cluster for the kSelect algorithm. All the functions below assume that the array is stored 
*  in a .txt file with the 1st line determining the size of the array, followed by a blank line and 
*  the array elements, separated by newlines.
*
*  The other portion of the file uses libcurl to retrieve a file using its URL and store it in RAM 
*  as an array of uint32_t elements.
*/
#ifndef ARRAY_PARSING_H
#define ARRAY_PARSING_H

#include <stdint.h>
#include <stdlib.h>

#define WIKI_MAX_PER_CURL 3

/* Simple struct to package array pointer and size*/ 
typedef struct{
  uint32_t* values;
  int local_size;
  uint64_t total_size;
} ARRAY;

// Byte streaming struct.
typedef struct {
  char* data;
  uint64_t current_byte_size;
  uint64_t total_byte_size;
} STREAM;

/*  
* Parsing of data matrix that is in shared memory (i.e the host's disk).
* Inputs:
*   file_name (string): the name of the array txt file (relative paths are supported).
*   slot_id (int): the id of the slot in the slot cluster.
*   int slot_count (int): total number of slots that share the file.
*   world_rank,world_size: determine the part of the array to be read.
* Outputs:
*   ARRAY: read array portion. 
*/
ARRAY sharedFileParsing(const char *file_name, int slot_id, int slot_count);

/* Functions used for getURLFile */

/* Simple write callback function for easy_perform */
size_t write_callback(void* data, size_t size, size_t nmemb, void* dest);

/* Get size of file (performed only by root) */
uint64_t getURLSize(const char *url);

/* Actual data retrieval from URL using the known size */ 
ARRAY getURLPartition(const char *url,int world_rank,int world_size,uint64_t total_size);

/*  Main function that syncs the 3 above. MaxThreadsPerCurl is specified so that retrieval time can 
*   be minimized without data loss.
*/
ARRAY getURLFile(const char *url, int world_rank, int world_size, int maxThreadsPerCurl);


#endif
