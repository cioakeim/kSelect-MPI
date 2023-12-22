/* Main function that tests the kSelect algorithm*/ 
#include <stdio.h>
#include <stdlib.h>
#include "kSelectParallel.h"
#include "tests.h"
#include "arrayParsing.h"
 

int main(int argc, char**argv){
  printf("Entering main\n");
  int val=kSelectParallel(argv[1],8);
  printf("Exited parallel\n");
  printf("Val %d\n",val);
  return 0;
}
