/*  This module constists of all the ways of parsing a desired array into the program.
 *  1. In txt format: array_size \n\n element[0] \n element [1] \n ... 
 *  2. In binary format. 
 *  3. Using libCURL to read the whole file as binary.
 *  4. Using libCURL to read a beginning portion of the file as binary until a specified size.
*/
#ifndef ARRAY_PARSING_H
#define ARRAY_PARSING_H

#include <stdint.h>
#include <stdlib.h>

// The dataset link only allows this many curl calls from the same ip.
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
* The data is in txt format where each line is a uint32_t and 1st line is size \n\n.
*/
ARRAY sharedFileParsing(const char *file_name, int slot_id, int slot_count);

/* Given a file name, each task gets its corresponding cut*/
ARRAY sharedFileBinaryParsing(const char* file_name, int world_rank, int world_size);

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

/* Modified version of getURLFile that lets the user specify the size of the array that they want
* so that curl only scans the beginning of the array that corresponds to that size. 
*/
ARRAY getURLFixedSize(const char *url,int world_rank,int world_size,int maxThreadsPerCurl,uint64_t total_size);

#endif
