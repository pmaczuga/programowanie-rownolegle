#!/bin/bash -l

for size in $3 $4 $5
do
    for ((i=0; i<$1; i++))
    do
        ./pi.sh $size $2
        size_scale=$(expr "$size" "/" "$2")
        ./pi_scale.sh $size_scale $2
    done
done

