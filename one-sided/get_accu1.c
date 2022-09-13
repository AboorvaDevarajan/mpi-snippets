#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT 10000

int main(int argc, char **argv) {

  MPI_Init(&argc, &argv);

  int i, rank, nproc, peer, count;
  MPI_Win win;

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  {
    for (count = 0; count <= COUNT; count++) {

      int *win_ptr = malloc(sizeof(int) * count);
      int *res_ptr = malloc(sizeof(int) * count);
      int *origin_ptr = malloc(sizeof(int) * count);

      peer = (rank + 1) % nproc;

      int i;
      for (i = 0; i < count; i++) {
        win_ptr[i] = rank;
        origin_ptr[i] = rank * 100;
        res_ptr[i] = rank * 100;
      }

      MPI_Win_create(win_ptr, sizeof(int) * count, sizeof(int),
                     MPI_INFO_NULL, MPI_COMM_WORLD, &win);

      MPI_Datatype contig_vector;
      MPI_Type_vector(count, 1, 1, MPI_INT, &contig_vector);
      MPI_Type_commit(&contig_vector);

      if (rank % 2 == 0) {
        MPI_Win_fence(0, win);
        MPI_Get_accumulate(origin_ptr, 1, contig_vector, res_ptr, 1,
                           contig_vector, peer, 0, 1, contig_vector, MPI_SUM,
                           win);
        MPI_Win_fence(0, win);
        for (i = 0; i < count; i++) {
          if (res_ptr[i] != peer || origin_ptr[i] != (rank * 100) ||
              win_ptr[i] != rank) {
            printf("CASE 2: rank : %d result : %d expected : %d win_ptr : %d "
                   "expected: %d origin_ptr %d expected: %d\n",
                   rank, res_ptr[i], peer, win_ptr[i], rank, origin_ptr[i],
                   rank * 100);
            exit(1);
          }
        }
      } else {
        MPI_Win_fence(0, win);
        MPI_Win_fence(0, win);
        for (i = 0; i < count; i++) {
          if (res_ptr[i] != (rank * 100) || origin_ptr[i] != (rank * 100) ||
              win_ptr[i] != (((rank - 1) * 100) + rank)) {
            printf("CASE 2: rank : %d result : %d expected : %d win_ptr : %d "
                   "expected: %d origin_ptr %d expected: %d\n",
                   rank, res_ptr[i], (rank * 100), win_ptr[i],
                   (((rank - 1) * 100) + rank), origin_ptr[i], rank * 100);
            exit(1);
          }
        }
      }

      if (rank == 0)
        printf("CASE 2: count: %d PASS\n", count);

      MPI_Type_free(&contig_vector);
      MPI_Win_free(&win);
      free(win_ptr);
      free(res_ptr);
      free(origin_ptr);
      MPI_Barrier(MPI_COMM_WORLD);
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}
