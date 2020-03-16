#!/bin/bash -l
#SBATCH --nodes 1
#SBATCH --ntasks 16
#SBATCH --exclusive
#SBATCH --time=01:00:00
#SBATCH --partition=plgrid
#SBATCH --account=plgpmaczuga2020a

module add plgrid/tools/openmpi/1.6.5-gnu-4.9.2-ib
mpicc par.c -o par -lm

for ((i=1; i<=$2; i++)); do
    echo "Calculating for $i processes"
    mpiexec -np $i ./par $1
done
