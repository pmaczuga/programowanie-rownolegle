#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MB 1024 * 1024
#define KB 1024

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int *buf = calloc(10 * MB * 12, sizeof(int));
  int *recv = calloc(10 * MB, sizeof(int));
  int each_size = 10 * MB;

  int size = each_size * world_size;

  MPI_Barrier(MPI_COMM_WORLD);

  float start, end, my_time;
  start = MPI_Wtime();

  MPI_Scatter(buf, each_size, MPI_INT, recv, each_size, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Gather(recv, each_size, MPI_INT, buf, each_size, MPI_INT, 0, MPI_COMM_WORLD);

  end = MPI_Wtime();
  my_time = (end - start) * 1000.0;
  printf("Scatter for %d: %f ms\n", world_rank, my_time);


  MPI_Barrier(MPI_COMM_WORLD);
  free(buf);
  free(recv);

  MPI_Finalize();
}
