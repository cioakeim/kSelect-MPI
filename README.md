# kSelect-MPI
Implementation of the quick select algorithm using MPI.

## File structure
Folders "src" and "include" contain the source and header files respectively of the functions that are
usable for the main scripts. In more detail:
* "kSelectSequential" contains all the methods for the implementation of the sequential kSelect. 
* "kSelectParallel" contains all the parallel methods. Uses kSelectSequential's methods.
* "arrayParsing" is used for getting the ARRAY structure to input to the above algorithms.
* "testing" is just a bunch of scripts used to validate the correctness of the kSelect implementations.

The folder "main" contains all the scripts that use the above functions and were given to Aristotelis:
* "main" (./main [fileName] [k]) calculates and prints the result of kSelectParallel for some given [k],
  when the array given is [fileName] and is read as a binary.
* "time_n_job" is a script that produces the vectors for the plot ExecutionTime(arraySize).
* "time_p_job" similarly provides the average time for reading a file (used with batch scripts for time(p))
* "find_even_percentiles" is a script that calculates and stores [p] percentiles of a where each value 
  is i times size/p, where i ranges from 0 to p-1.

The folder "scripts" contains all the job scripts for the HPC server.

All binaries are created in the bin folder and can be called from there.
