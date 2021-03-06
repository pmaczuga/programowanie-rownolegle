#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>


int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  srand(time(NULL) * world_rank);

  MPI_Barrier(MPI_COMM_WORLD);
  double start = MPI_Wtime();

  long count = atol(argv[1]);
  long my_count = count / world_size;
  long within_circle = 0;

  for (long i = 0; i < my_count; i++) {
      double x = (double)rand() / (double)RAND_MAX;
      double y = (double)rand() / (double)RAND_MAX;

      double distance = x*x + y*y;

      if (distance <= 1.0) {
          within_circle++;
      }
  }

  long sum = 0;
  MPI_Reduce( &within_circle, &sum, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD );
  double end = MPI_Wtime();

  if (world_rank == 0) {
    double time_taken = (end - start) * 1000.0;

    double pi = (double)sum / (double)count;
    pi = 4.0 * pi;
    // printf("PI = %.15lf\n", pi);
    // printf("It took: %lf ms\n\n", time_taken);
    // Problem size; processes; time
    printf("%ld; %d; %lf\n", count, world_size, time_taken);
  }

  MPI_Finalize();
}
