#!/bin/bash

# Script used to extract the plots of time(n) for the kSelect algorithm.
# Used as a template for the master job that alters the script using sed.

#SBATCH --partition=rome
#SBATCH --job-name=kSelTimeN
#SBATCH --ntasks-per-node=4
#SBATCH --nodes=1
#SBATCH --time=00:25:00
#SBATCH --mem-per-cpu=2000
#SBATCH --output=timeN_.stdout

module load gcc/9.2.0 openmpi/4.0.3 curl/7.72.0
cd $HOME/kSelect-MPI

make timeN 

mkdir -p $HOME/plotData
srun $HOME/kSelect-MPI/bin/time_n_job $HOME/plotData/
