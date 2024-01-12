#!/bin/bash 

# Master that creates the job scripts needed and runs them.

#SBATCH --partition=testing
#SBATCH --job-name=kSelNMaster
#SBATCH --array 0-9

nTasks=$((2**$SLURM_ARRAY_TASK_ID))

mkdir -p $HOME/spawned_jobs
cp $HOME/kSelect-MPI/scripts/time_n_test.sh $HOME/spawned_jobs/time_n_test_$nTasks.sh 

if [ $nTasks -lt 32 ]; then 
  nNodes=1
elif [ $nTasks -lt 128 ]; then
  nNodes=2 
else
  nNodes=4 
fi

nTasksPerNode=$(($nTasks / $nNodes))

sed -i "s/--nodes=.*/--nodes=$nNodes/" $HOME/spawned_jobs/time_n_test_$nTasks.sh 
sed -i "s/--ntasks-per-node=.*/--ntasks-per-node=$nTasksPerNode/" $HOME/spawned_jobs/time_n_test_$nTasks.sh
sed -i "s/--output=timeN_.stdout/--output=timeN_$nTasksPerNode.stdout/" $HOME/spawned_jobs/time_n_test_$nTasks.sh
if [ $nTasks -lt 8 ]; then
  sed -i "s/--partition=rome/--partition=batch/" $HOME/spawned_jobs/time_n_test_$nTasks.sh 
  sed -i "s/--time=00:25:00/--time=00:30:00/" $HOME/spawned_jobs/time_n_test_$nTasks.sh
fi

sbatch $HOME/spawned_jobs/time_n_test_$nTasks.sh

