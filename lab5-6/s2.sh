#!/bin/bash
for threads in 1 2 3 4
do
    for size in 10000 100000 1000000
    do
        for div in 8 4 2 1
        do
            
            for (( i=0; i<$((10000000 / size)); i++ ))
            do
                ./bucket_sort_1 $threads $size $((size / div)) 0 100000000 >> a1.txt
                ./bucket_sort_2 $size $threads $((size / div)) >> a2.txt
            done
        done
    done
done

