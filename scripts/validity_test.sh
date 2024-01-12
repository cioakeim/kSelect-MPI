#!/bin/bash 

# This script is used to test the validity of the kSelect algorithm in a multi-node scenario
# using Slurm.

#SBATCH --partition=rome
#SBATCH --job-name=kSelectCorrectness
#SBATCH --ntasks-per-node=16
#SBATCH --nodes=2
#SBATCH --time=00:25:00

module load gcc/9.2.0 openmpi/4.0.3 curl/7.72.0 
cd $HOME/kSelect-MPI

make testing

srun $HOME/kSelect-MPI/bin/testing_job $HOME/temp.txt 1>$HOME/correctness_stdout.out

rm $HOME/temp.txt
make clean
