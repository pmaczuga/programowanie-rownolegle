#define _POSIX_C_SOURCE 1

#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    srand(time(NULL));


    if (argc < 4)
    {
        printf("random_vetor [threads] [size] [schedule] [chunk]\n");
        return 0;
    }

    int threads = atoi(argv[1]);
    int size = atoi(argv[2]);
    int chunk = atoi(argv[4]);
    char schedule[20];
    strcpy(schedule, argv[3]);

    omp_set_num_threads(threads);
    int i;
    int *vector = malloc(size * sizeof(int));
    
    double start, end, time_taken;
    unsigned int seed;

    if (strcmp(schedule, "static") == 0)
    {
        start = omp_get_wtime();
        #pragma omp parallel shared(vector, chunk) private(i, seed)
        {
            seed = time(NULL);
            #pragma omp for schedule(static, chunk) nowait
                for(i=0; i < size; i++)
                {
                    vector[i] = rand_r(&seed)%1000;
                }
        }
        end = omp_get_wtime();
    } 
    else if (strcmp(schedule, "dynamic") == 0)
    {

        start = omp_get_wtime();
        #pragma omp parallel shared(vector, chunk) private(i, seed)
        {
            seed = time(NULL);
            #pragma omp for schedule(dynamic, chunk) nowait
                for(i=0; i < size; i++)
                {
                    vector[i] = rand_r(&seed)%1000;
                }
        }
        end = omp_get_wtime();
    } 
     else if (strcmp(schedule, "guided") == 0)
    {

        start = omp_get_wtime();
        #pragma omp parallel shared(vector, chunk) private(i, seed)
        {
            seed = time(NULL);
            #pragma omp for schedule(guided, chunk) nowait
                for(i=0; i < size; i++)
                {
                    vector[i] = rand_r(&seed)%1000;
                }
        }
        end = omp_get_wtime();
    } 
     else if (strcmp(schedule, "runtime") == 0)
    {

        start = omp_get_wtime();
        #pragma omp parallel shared(vector, chunk) private(i, seed)
        {
            seed = time(NULL);
            #pragma omp for schedule(runtime) nowait
                for(i=0; i < size; i++)
                {
                    vector[i] = rand_r(&seed)%1000;
                }
        }
        end = omp_get_wtime();
    } 
    else
    {
        printf("Wrong chedule: %s. Has to be one of: static, dynamic, guided, runtime\n", schedule);
        return 1;
    }

    time_taken = (end-start)*1000;
    printf("%d;%d;%s,%d;%0.2lf\n", threads, size, schedule, chunk, time_taken);

    free(vector);
    return 0;
}
