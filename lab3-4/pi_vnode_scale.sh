#!/bin/bash -l

mpicc -std=c99 par.c -o par -lm

for ((i=1; i<=$2; i++)); do
    echo "Calculating for $i processes"
    size=$( expr "$i" '*' "$1")
    mpiexec --machinefile allnodes -np $i ./par $size
done
