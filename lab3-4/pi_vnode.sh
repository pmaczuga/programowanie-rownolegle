#!/bin/bash -l

mpicc -std=c99 par.c -o par -lm

for ((i=1; i<=$2; i++)); do
    echo "Calculating for $i processes"
    mpiexec --machinefile allnodes -np $i ./par $1
done
