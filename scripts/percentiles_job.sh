#!/bin/bash

# Script used to extract the plots of time(n) for the kSelect algorithm

#SBATCH --partition=rome
#SBATCH --job-name=kSelPercentiles
#SBATCH --ntasks-per-node=64
#SBATCH --nodes=4
#SBATCH --time=00:05:00
#SBATCH --mem-per-cpu=2000
#SBATCH --output=percentiles_.stdout

module load gcc/9.2.0 openmpi/4.0.3 curl/7.72.0
cd $HOME/kSelect-MPI

make percentiles 

mkdir -p $HOME/plotData
srun $HOME/kSelect-MPI/bin/find_even_percentiles $HOME/englishWiki.tar.7z 16 $HOME/plotData
