#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define KB 1024
#define MB 1024 * 1024

#define SAMPLE_COUNT 100000
#define SMALL_BUF_SIZE 5

#define SEND_FOR_SECONDS 10.0
#define MIN_BUF_SIZE 512 * KB
#define MAX_BUF_SIZE 32 * MB
#define STEP 512 * KB

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  
  int src = 0;
  int dest = 1;
  int tag = 0;
  char filename[100];

  // Parse args
  if (argc < 2) {
    printf("Not enough arguments\n. Required:\n");
    printf("out_file\n");
  } else {
    strcpy(filename, argv[1]);
  }

  FILE *fp;
  fp = fopen(filename, "w");

  // We are assuming at least 2 processes for this task
  if (world_size < 2) {
    fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1); 
  }


  // --------------------------------------------------------------
  // |                     Calculate delay                        |
  // --------------------------------------------------------------

  // Standard Send
  MPI_Barrier(MPI_COMM_WORLD);
  if (world_rank == 0) {
    printf("Calculating Standard Send delay...");
    double time = 0.0;
    char small_data[SMALL_BUF_SIZE] = {'d', 'a', 't', 'a', '\0'};
    int count = SAMPLE_COUNT;

    time -= MPI_Wtime();
    for (int i=0; i<count; i++) {
      MPI_Send(small_data, SMALL_BUF_SIZE, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
      MPI_Recv(small_data, SMALL_BUF_SIZE, MPI_CHAR, dest, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    time += MPI_Wtime();

    double delay = time * (1000.0 / (SAMPLE_COUNT * 2.0));
    fprintf(fp, "%.20f\n", delay);
    printf(" DONE!\n");
  }
  // Receive
  if (world_rank == 1) {
    char received[SMALL_BUF_SIZE];
    int count = SAMPLE_COUNT;

    for (int i=0; i<count; i++) {
      MPI_Recv(received, SMALL_BUF_SIZE, MPI_CHAR, src, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Send(received, SMALL_BUF_SIZE, MPI_CHAR, src, tag, MPI_COMM_WORLD);
    }
  }

  // Synchronous Send
  MPI_Barrier(MPI_COMM_WORLD);
  if (world_rank == 0) {
    printf("Calculating Synchronous Send delay...");
    double time = 0.0;
    char small_data[5] = {'d', 'a', 't', 'a', '\0'};
    int count = SAMPLE_COUNT;

    time -= MPI_Wtime();
    for (int i=0; i<count; i++) {
      MPI_Ssend(small_data, SMALL_BUF_SIZE, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
      MPI_Recv(small_data, SMALL_BUF_SIZE, MPI_CHAR, dest, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    time += MPI_Wtime();

    double delay = time * (1000.0 / (SAMPLE_COUNT * 2.0));
    fprintf(fp, "%.20f\n", delay);
    printf(" DONE!\n");
  }
  // Receive
  if (world_rank == 1) {
    char received[SMALL_BUF_SIZE];
    int count = SAMPLE_COUNT;

    for (int i=0; i<count; i++) {
      MPI_Recv(received, SMALL_BUF_SIZE, MPI_CHAR, src, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Ssend(received, SMALL_BUF_SIZE, MPI_CHAR, src, tag, MPI_COMM_WORLD);
    }
  }


  // --------------------------------------------------------------
  // |                     Calculate bandwidth                    |
  // --------------------------------------------------------------

  // Standard Send
  MPI_Barrier(MPI_COMM_WORLD);
  if (world_rank == 0) {
    printf("\n");
    int data_size = MIN_BUF_SIZE;
    while (data_size <= MAX_BUF_SIZE) {
      printf("Calculating Standard Send bandwidth for %d B...", data_size);
      char *data = calloc(data_size, sizeof(char));
      double time = 0.0;
      int bandwidth = 0;
      int count = 0;

      while (time <= SEND_FOR_SECONDS) {
        time -= MPI_Wtime();
        MPI_Send(data, data_size, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
        MPI_Recv(data, data_size, MPI_CHAR, dest, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        time += MPI_Wtime();
        count++;
      }
      data[0] = 'f';
      MPI_Send(data, data_size, MPI_CHAR, dest, tag, MPI_COMM_WORLD); // Tell that finished
      bandwidth = (int) (data_size * count * 8) / (time * MB);

      fprintf(fp, "%d; %d\n", data_size, bandwidth);
      data_size += STEP;
      free(data);
      printf(" DONE!\n");
    }
    fprintf(fp, "\n");
  }
  if (world_rank == 1) {
    int data_size = MIN_BUF_SIZE;
    while (data_size <= MAX_BUF_SIZE) {
      char *received = malloc(data_size * sizeof(char));

      while (1) {
        MPI_Recv(received, data_size, MPI_CHAR, src, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (received[0] == 'f')
          break;
        MPI_Send(received, data_size, MPI_CHAR, src, tag, MPI_COMM_WORLD);
      }

      free(received);
      data_size += STEP;
    }
  }

  // Synchronous Send
  MPI_Barrier(MPI_COMM_WORLD);
  if (world_rank == 0) {
    printf("\n");
    int data_size = MIN_BUF_SIZE;
    while (data_size <= MAX_BUF_SIZE) {
      printf("Calculating Synchronous Send bandwidth for %d B...", data_size);
      char *data = calloc(data_size, sizeof(char));
      double time = 0.0;
      int bandwidth = 0;
      int count = 0;

      while (time <= SEND_FOR_SECONDS) {
        time -= MPI_Wtime();
        MPI_Ssend(data, data_size, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
        MPI_Recv(data, data_size, MPI_CHAR, dest, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        time += MPI_Wtime();
        count++;
      }
      data[0] = 'f';
      MPI_Send(data, data_size, MPI_CHAR, dest, tag, MPI_COMM_WORLD); // Tell that finished
      bandwidth = (int) (data_size * count * 8) / (time * MB);

      fprintf(fp, "%d; %d\n", data_size, bandwidth);
      data_size += STEP;
      free(data);
      printf(" DONE!\n");
    }
  }
  if (world_rank == 1) {
    int data_size = MIN_BUF_SIZE;
    while (data_size <= MAX_BUF_SIZE) {
      char *received = malloc(data_size * sizeof(char));

      while (1) {
        MPI_Recv(received, data_size, MPI_CHAR, src, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (received[0] == 'f')
          break;
        MPI_Ssend(received, data_size, MPI_CHAR, src, tag, MPI_COMM_WORLD);
      }

      free(received);
      data_size += STEP;
    }
  }

  fclose(fp);
  MPI_Finalize();
}
