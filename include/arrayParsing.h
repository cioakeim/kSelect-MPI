/*  
*  This module is used for the parsing of the array (that is stored in disk) to the nodes of the 
*  MPI cluster for the kSelect algorithm. All the functions below assume that the array is stored 
*  in a .txt file with the 1st line determining the size of the array, followed by a blank line and 
*  the array elements, separated by newlines.
*/
#ifndef ARRAY_PARSING_H
#define ARRAY_PARSING_H

/* Simple struct to package array pointer and size*/ 
typedef struct{
  int* values;
  int local_size;
  int total_size;
} ARRAY;

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
ARRAY sharedFileParsing(char *file_name, int slot_id, int slot_count);



#endif
