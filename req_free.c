#include "mpi.h"
#include <stdlib.h>
#define size 20
int main(int argc, char *argv[])
{
    MPI_Request r;
    MPI_Status s;
    int flag;
    int buf[size];
    int rbuf[size];
    int tag = 27;
    int dest = 0;
    int rank, sz, i;
    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &sz );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    while(1) {
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Isend( buf, size, MPI_INT, rank, tag, MPI_COMM_WORLD, &r );
        MPI_Request_free( &r );
        MPI_Recv( rbuf, size, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &s );
    }
    MPI_Finalize();
    return 0;
}
