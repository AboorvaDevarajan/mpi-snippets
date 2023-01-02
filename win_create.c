
#include <mpi.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
        int rank, size;
        int* buffer;
        MPI_Win Win;
        MPI_Aint sizeOfInt, lb;

        MPI_Init(&argc, &argv);

        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);

        if (rank == 0) {
                buffer = NULL;
        } else {
                MPI_Alloc_mem(sizeof(int) * rank, MPI_INFO_NULL, &buffer);
        }

        MPI_Type_get_extent(MPI_INT, &lb, &sizeOfInt);
        MPI_Win_create(buffer, sizeOfInt * rank, 1, MPI_INFO_NULL,
                        MPI_COMM_WORLD, &Win);
        MPI_Win_free(&Win);

        if (rank != 0)  MPI_Free_mem(buffer);
        MPI_Finalize();
        return 0;
}
