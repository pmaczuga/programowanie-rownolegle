#define _POSIX_C_SOURCE 1

#include "bucket.h"

#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("%s [threads] [size] [chunk] [buckets]\n", argv[0]);
        return 0;
    }

    int threads = atoi(argv[1]);        // Number of threads
    int size = atoi(argv[2]);           // Size of vector to sort
    int chunk = atoi(argv[3]);          // Size of chunk - used in schedule
    int buckets_count = atoi(argv[4]);  // Number of buckets

    omp_set_num_threads(threads);
    int i, j, bucket_index, range_index;
    double normalized;
    int *vector = malloc(size * sizeof(int));
    int random_range = 10000000;
    
    unsigned int seed;

    printf("Threads: %d, Size: %d, chunk: %d\n", threads, size, chunk);

    Bucket **buckets = malloc(sizeof(Bucket *) * threads);  // Array of buckets for every thread
    int **elements_count = malloc(sizeof(int *) * threads);   // Array of size [threads][threads]. Number of elements in range for each thread.
                                                            // For instance elements_count[2][0] represet number of smallest elements that was
                                                            // counted by thread number 2
    #pragma omp parallel shared(vector, chunk, buckets, elements_count) private(i, j, seed, bucket_index, range_index, normalized)
    {
        int thread_num = omp_get_thread_num();
        seed = ((int) time(NULL)) ^ thread_num;
 
        // Initialzie buckets and elements_count
        buckets[thread_num] = malloc(sizeof(Bucket) * buckets_count);
        for (i=0; i < buckets_count; i++)
        {
            bucket_init(&buckets[thread_num][i]);
        }
        elements_count[thread_num] = malloc(sizeof(int) * threads);
        for (i=0; i < threads; i++)
        {
            elements_count[thread_num][i] = 0;
        }
        #pragma omp barrier

        // Fill vector with random int values
        #pragma omp for schedule(static, chunk)
            for (i=0; i < size; i++)
            {
                vector[i] = rand_r(&seed)%random_range;
            }
        

        // Insert values to proper buckets
        # pragma omp for schedule(static, chunk)
            for (i=0; i < size; i++)
            {
                normalized = ((double) vector[i]) / ((double) random_range);
                bucket_index = (int) (normalized * buckets_count);
                bucket_insert(&buckets[thread_num][bucket_index], vector[i]);
                range_index = bucket_index / (buckets_count / threads);
                if (range_index == threads)
                    range_index = threads - 1;
                elements_count[thread_num][range_index]++;
            }

        // Merge buckets
        # pragma omp for schedule(static, chunk)
            for (i=0; i < buckets_count; i++)
            {
                for (j=1; j < threads; j++)
                {
                    bucket_merge(&buckets[0][i], &buckets[j][i]);
                }
            }
        for (j=1; j<threads; j++)
        {
            elements_count[0][thread_num] += elements_count[j][thread_num];
        }

        if (thread_num == 0)
        {
            printf("Elements in buckets:\n");
            for (i=0; i < buckets_count; i++)
            printf("%d\n", buckets[0][i].size);
        }

        #pragma omp barrier 

        free(buckets[thread_num]);
        free(elements_count[thread_num]);
    }
    free(buckets);
    free(elements_count);

    free(vector);
    return 0;
}
