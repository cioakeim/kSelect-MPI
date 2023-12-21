/* Main function that tests the kSelect algorithm*/ 
#include <stdio.h>
#include <stdlib.h>
#include "kSelectParallel.h"
#include "tests.h"
#include "arrayParsing.h"
 

int main(int argc, char**argv){
  int val=kSelectParallel(argv[1],8);
  printf("Val %d\n",val);
  return 0;
}
