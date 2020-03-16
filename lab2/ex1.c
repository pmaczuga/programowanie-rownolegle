#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>



int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int buf[100];
  int recv[100];

  int size = 3 * world_size;

  if (world_rank == 0) {
    for (int i = 0; i < size; i++) {
      buf[i] = 1;
    }
  } 

  MPI_Scatter(buf, 3, MPI_INT, recv, 3, MPI_INT, 0, MPI_COMM_WORLD);

  for (int i = 0; i < 3; i++) {
    recv[i] *= world_rank;
  }

  MPI_Gather(recv, 3, MPI_INT, buf, 3, MPI_INT, 0, MPI_COMM_WORLD);

  if (world_rank == 0) {
    for (int i = 0; i < size; i++) {
      printf("%d ", buf[i]);
    }
    printf("\n");
  }

  MPI_Finalize();
}
