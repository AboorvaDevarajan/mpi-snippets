#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT 100000

int main(int argc, char **argv) {

  MPI_Init(&argc, &argv);

  int i, rank, nproc, peer, count, count1;
  MPI_Win win;

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);


  // CASE: MPI_NO_OP origin : derived dt (non-contig) source : derived dt
  // (non-contig) target: derived dt (non-contig)
  {
    for (i = 0; i <= 2; i++) {
      count = COUNT;
      int *win_ptr = malloc(sizeof(int) * (2 * count));
      int *res_ptr = malloc(sizeof(int) * (2 * count));
      int *origin_ptr = malloc(sizeof(int) * (2 * count));

      peer = (rank + 1) % nproc;

      int i;
      for (i = 0; i < (2 * count); i++) {
        if (i % 2 == 0) {
           win_ptr[i] = rank;
           origin_ptr[i] = rank * 1000;
           res_ptr[i] = rank * 100;
        } else {
            win_ptr[i] = 0;
           origin_ptr[i] = rank * 100;
           res_ptr[i] = rank * 1000;
        }
      }

      MPI_Win_create(win_ptr, sizeof(int) * (2 * count), sizeof(int),
                     MPI_INFO_NULL, MPI_COMM_WORLD, &win);

      MPI_Datatype non_contig_vector;
      MPI_Type_vector(count, 1, 2, MPI_INT, &non_contig_vector);
      MPI_Type_commit(&non_contig_vector);
      // NO-OP check
      if (rank % 2 == 0) {
        MPI_Win_fence(0, win);
        MPI_Get_accumulate(origin_ptr, 1, non_contig_vector, res_ptr, 1,
                           non_contig_vector, peer, 0, 1, non_contig_vector,
                           MPI_NO_OP, win);
        MPI_Win_fence(0, win);
        for (i = 0; i < (2 * count); i++) {
          if (i % 2 == 0) {
            if (res_ptr[i] != peer || origin_ptr[i] != (rank * 1000) ||
                win_ptr[i] != rank) {
              printf("CASE 31: rank : %d result : %d expected : %d win_ptr : %d "
                     "expected: %d origin_ptr %d expected: %d\n",
                     rank, res_ptr[i], peer, win_ptr[i], rank, origin_ptr[i],
                     rank * 1000);
              exit(1);
            }
          } else {
            if (res_ptr[i] != (rank * 1000) || origin_ptr[i] != (rank * 100) ||
                win_ptr[i] != 0) {
              printf("CASE 32: rank : %d result : %d expected : %d win_ptr : %d "
                     "expected: %d origin_ptr %d expected: %d\n",
                     rank, res_ptr[i], (rank * 100), win_ptr[i], 0,
                     origin_ptr[i], rank * 100);
              exit(1);
            }
          }
        }

      } else {
        MPI_Win_fence(0, win);
        MPI_Win_fence(0, win);
        for (i = 0; i < (2 * count); i++) {
          if (i % 2 == 0) {
            if (res_ptr[i] != (rank * 100) || origin_ptr[i] != (rank * 1000) ||
                win_ptr[i] != rank) {
              printf("CASE 33: rank : %d result : %d expected : %d win_ptr : %d "
                     "expected: %d origin_ptr %d expected: %d\n",
                     rank, res_ptr[i], (rank * 100), win_ptr[i], rank,
                     origin_ptr[i], rank * 1000);
              exit(1);
            }
          } else {
            if (res_ptr[i] != (rank * 1000) || origin_ptr[i] != (rank * 100) ||
                win_ptr[i] != 0) {
              printf("CASE 34: rank : %d result : %d expected : %d win_ptr : %d "
                     "expected: %d origin_ptr %d expected: %d\n",
                     rank, res_ptr[i], (rank * 1000), win_ptr[i], 0,
                     origin_ptr[i], rank * 100);
              exit(1);
            }
          }
        }
      }

      if (rank == 0)
        printf("CASE 3: count: %d PASS\n", count);


      MPI_Type_free(&non_contig_vector);
      MPI_Win_free(&win);
      free(win_ptr);
      free(res_ptr);
      free(origin_ptr);
      MPI_Barrier(MPI_COMM_WORLD);
    }
  }


//=================
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Finalize();
  return 0;
}

