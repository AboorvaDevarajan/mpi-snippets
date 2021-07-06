
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MIN_WINSIZE	0
#define MAX_WINSIZE	1024
#define WINSTEP		64

static int		locbuf[MAX_WINSIZE];
static int		buf[MAX_WINSIZE];

#define MAX_ITERS	1

int
main(int argc, char **argv)

{
    int		rank, size, i, j, k;
    MPI_Aint	shmsize;
    int		*shmbuf;
    MPI_Win		win;
    MPI_Group	group;
    MPI_Group	wgroup;
    int		res;
    int		wsize;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (wsize = MIN_WINSIZE; wsize <= MAX_WINSIZE; wsize += WINSTEP) {
        shmsize = wsize * sizeof(int);
        MPI_Alloc_mem(shmsize, MPI_INFO_NULL, &shmbuf);
        MPI_Win_create(shmbuf, shmsize, sizeof(int),
        MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        MPI_Barrier(MPI_COMM_WORLD);
        j = (rank + 1) % size;
        MPI_Win_fence(0, win);
        MPI_Get(locbuf, wsize, MPI_INT, j, 0, wsize, MPI_INT, win);
        MPI_Win_fence(0, win);
        MPI_Win_free(&win);
        if (win != MPI_WIN_NULL) {
            MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
        }
        MPI_Free_mem(shmbuf);
    }
    MPI_Finalize();
    if (rank == 0) printf("passed\n");
    exit(0);
}
