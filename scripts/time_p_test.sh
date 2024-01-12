#!/bin/bash

# Script used to extract the plots of time(n) for the kSelect algorithm

#SBATCH --partition=rome
#SBATCH --job-name=kSelTimeN
#SBATCH --ntasks-per-node=4
#SBATCH --nodes=1
#SBATCH --time=00:15:00
#SBATCH --mem-per-cpu=4000
#SBATCH --output=timeP_.stdout

module load gcc/9.2.0 openmpi/4.0.3 curl/7.72.0
cd $HOME/kSelect-MPI

make timeP 

mkdir -p $HOME/plotData
srun $HOME/kSelect-MPI/bin/time_p_job $HOME/plotData $HOME/englishWiki.tar.7z
