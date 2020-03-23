#!/bin/bash -l
#SBATCH --nodes 1
#SBATCH --ntasks 12
#SBATCH --exclusive
#SBATCH --time=01:00:00
#SBATCH --partition=plgrid
#SBATCH --account=plgpmaczuga2020a

module add plgrid/tools/openmpi/1.6.5-gnu-4.9.2-ib
mpicc -std=c99 par.c -o par -lm

echo "Scalling. Size: $1, Max processes: $2"
for ((i=1; i<=$2; i++)); do
    size=$( expr "$i" '*' "$1")
    mpiexec -np $i ./par $size
done
