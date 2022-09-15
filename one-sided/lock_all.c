#include <stdio.h>
#include <string.h>
#include <mpi.h>

int main (int argc, char *argv[]) {
    MPI_Datatype datatype;
    MPI_Win win;
    MPI_Info info;
    int size,rank;

    MPI_Init(&argc, &argv);
    int loc = -1;
    int * shared;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size %2) MPI_Abort(MPI_COMM_WORLD, 77);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int peer = rank^1;

    MPI_Info_create(&info);
    MPI_Info_set(info, "alloc_shm", "true");
    MPI_Alloc_mem(sizeof(int), info, &shared);
    MPI_Win_create(shared, sizeof(int), 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    *shared = rank;

    //MPI_Win_lock_all(MPI_MODE_NOCHECK, win);
    MPI_Win_lock_all(0, win);

    MPI_Type_dup(MPI_INT, &datatype);
    MPI_Type_commit(&datatype);

    MPI_Get_accumulate(NULL, 0, MPI_INT,
                       &loc, 1, datatype,
                       peer, 0, 1, MPI_INT, MPI_NO_OP, win);

    MPI_Win_unlock_all(win);
    MPI_Win_free(&win);
    MPI_Free_mem(shared);

    if (loc != peer)
        printf("rank %d: expected %d but got %d\n", rank, peer, loc);


    MPI_Finalize();
    return 0;
}