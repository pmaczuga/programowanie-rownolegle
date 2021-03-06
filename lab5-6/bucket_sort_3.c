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
    int i, j, bucket_index, range_index, current_elem;
    double normalized;
    int *vector = malloc(size * sizeof(int));
    int random_range = 10000000;
    
    double start[7], end[7], time_taken[7];
    unsigned int seed;

    printf("Threads: %d, Size: %d, chunk: %d, buckets: %d\n", threads, size, chunk, buckets_count);

    start[0] = omp_get_wtime();
    Bucket **buckets = malloc(sizeof(Bucket *) * threads);  // Array of buckets for every thread
    int **elements_count = malloc(sizeof(int *) * threads);   // Array of size [threads][threads]. Number of elements in range for each thread.
                                                            // For instance elements_count[2][0] represet number of smallest elements that was
                                                            // counted by thread number 2
    #pragma omp parallel shared(vector, chunk, buckets, elements_count) private(i, j, seed, bucket_index, range_index, normalized, current_elem)
    {
        int thread_num = omp_get_thread_num();
        if (thread_num == 0)
            start[6] = omp_get_wtime();
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
        if (thread_num == 0)
            end[6] = omp_get_wtime();

        // Fill vector with random int values
        if (thread_num == 0)
            start[1] = omp_get_wtime();
        #pragma omp for schedule(static, chunk)
            for (i=0; i < size; i++)
            {
                vector[i] = rand_r(&seed)%random_range;
            }
        if (thread_num == 0)
            end[1] = omp_get_wtime();
        

        // Insert values to proper buckets
        if (thread_num == 0)
            start[2] = omp_get_wtime();
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
        if (thread_num == 0)
            end[2] = omp_get_wtime();

        // Merge buckets
        if (thread_num == 0)
            start[3] = omp_get_wtime();
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
            end[3] = omp_get_wtime();

        // Sort buckets
        if (thread_num == 0)
            start[4] = omp_get_wtime();
        # pragma omp for schedule(static, chunk)
            for (i=0; i < buckets_count; i++)
            {
                bucket_sort(&buckets[0][i]);
            }
        if (thread_num == 0)
            end[4] = omp_get_wtime();

        // Calculate index and put back sorted
        if (thread_num == 0)
            start[5] = omp_get_wtime();

        // Calculate how many number are in buckets with indexes: 0 to (size/threads * thread_num)
        if (thread_num == 0)
        {
            for (j=1; j < threads; j++)
            {
                elements_count[0][j] += elements_count[0][j-1];
            }
        }
       #pragma omp barrier

        // Put sorted elements back to vector
        bucket_index = (buckets_count/threads)*(thread_num+1);
        if (thread_num == threads-1)
            bucket_index = buckets_count;
        if (thread_num == 0)
            j = 0;
        else
            j = elements_count[0][thread_num-1];

        for (i=(buckets_count/threads) * thread_num; i < bucket_index; i++)
        {
            current_elem = bucket_pop(&buckets[0][i]);
            while(current_elem >= 0)
            {
                vector[j] = current_elem;
                j++;
                current_elem = bucket_pop(&buckets[0][i]);
            }
        }
        #pragma omp barrier
        if (thread_num == 0)
            end[5] = omp_get_wtime();

        // Clean up
        free(buckets[thread_num]);
        free(elements_count[thread_num]);
    }
    free(buckets);
    free(elements_count);
    end[0] = omp_get_wtime();

    for (i=0; i < 7; i++)
        time_taken[i] = (end[i]-start[i])*1000;
    printf("\nTime of parts of program [ms]:\n");
    printf("Init time        : %lf\n", time_taken[6]);
    printf("Random fill time : %lf\n", time_taken[1]);
    printf("Split time       : %lf\n", time_taken[2]);
    printf("Merge time       : %lf\n", time_taken[3]);
    printf("Sort time        : %lf\n", time_taken[4]);
    printf("Put back time    : %lf\n", time_taken[5]);
    printf("Full time        : %lf\n", time_taken[0]);

    for (i=0; i < size-1; i++)
    {
        if (vector[i] > vector[i+1])
        {
            printf("ERROR: Data not sorted !!!\n");
            break;
        }
    }

    free(vector);
    return 0;
}
