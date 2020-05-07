#include <omp.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

double total_time, bucket_time, sort_time, merge_time, random_time;

struct bucket
{
    int count;
    double *values;
};

double *get_random_array(int n)
{
    int i;
    unsigned int seed;
    double time;
    double *array = malloc(sizeof(double) * n);

    time = omp_get_wtime();
#pragma omp parallel default(none) shared(array, n) private(seed)
    {
        seed = omp_get_thread_num();
#pragma omp for private(i)
        for(i = 0; i < n; i++) {
            array[i] = ((double) rand_r(&seed) / RAND_MAX);
        }
    }
    random_time = omp_get_wtime() - time;
    return array;
}

int compare(const void *first, const void *second)
{
    double a = *((double *)first);
    double b = *((double *)second);
    if (a < b)
        return -1;
    else if (a > b)
        return 1;
    else
        return 0;
}

void bucket_sort(double *array, int n, int bucket_count)
{
    double bucket_size = 1.0 / (double)bucket_count;
    double start, end;

    struct bucket *buckets = malloc(sizeof(struct bucket) * bucket_count);
    omp_lock_t *bucket_locks = malloc(sizeof(omp_lock_t) * bucket_count);
    int *indexes = malloc(sizeof(int) * bucket_count);

    int i, j;

#pragma omp parallel for default(shared) private(i)
    for (i = 0; i < bucket_count; i++)
    {
        buckets[i].count = 0;
        buckets[i].values = malloc(sizeof(double) * 12);
        omp_init_lock(&bucket_locks[i]);
    }

    start = omp_get_wtime();

#pragma omp parallel for shared(array, buckets)
    for (i = 0; i < n; i++)
    {
        int id = array[i] / bucket_size;

        omp_set_lock(&bucket_locks[id]);
        buckets[id].values[buckets[id].count++] = array[i];
        omp_unset_lock(&bucket_locks[id]);
    }
    end = omp_get_wtime();
    bucket_time = end - start;

    start = omp_get_wtime();
#pragma omp parallel for default(shared) private(i)
    for (i = 0; i < bucket_count; i++)
    {
        qsort(buckets[i].values, buckets[i].count, sizeof(double), &compare);
    }
    end = omp_get_wtime();
    sort_time = end - start;

    start = omp_get_wtime();
    indexes[0] = 0;
    for (i = 1; i < bucket_count; i++)
    {
        indexes[i] = indexes[i - 1] + buckets[i - 1].count;
    }

#pragma omp parallel for default(shared) private(i, j)
    for (i = 0; i < bucket_count; i++)
    {
        for (j = 0; j < buckets[i].count; j++)
        {
            array[indexes[i] + j] = buckets[i].values[j];
        }
    }
    end = omp_get_wtime();
    merge_time = end - start;
}

int main(int argc, char **argv)
{
    if(argc != 4) {
        printf("Usage: %s <array_size> <threads> <buckets>\n", argv[0]);
        exit(-1);
    }

    int n = atoi(argv[1]);
    int threads = atoi(argv[2]);
    double start;
    double end;
    int buckets = atoi(argv[3]);


    omp_set_dynamic(0);
    omp_set_num_threads(threads);

    start = omp_get_wtime();

    double *array = get_random_array(n);

    bucket_sort(array, n, buckets);

    end = omp_get_wtime();

    total_time = end - start;

    int i;
    for(i = 0 ; i < n - 1; i++) {
        if(array[i] > array[i+1]) {
            printf("%s\n%d\n", "Array not sorted", i);
            return -2;
        }
    }

    printf("Array size: %d, Buckets: %d, Threads: %d, Total time: %lf, Random time: %lf, Bucket time: %lf, Sort time: %lf, Merge time: %lf\n",
           n, buckets, threads, total_time, random_time, bucket_time, sort_time, merge_time);

    return 0;
}
