
#include "mpi.h"
#include <stdio.h>
#include<cuda.h>
#include<cuda_runtime.h>
// compile: mpicc -I/usr/local/cuda/include -L/usr/local/cuda/lib64 -lcuda -lcudart ddt.c
int main(int argc, char *argv[])
{

    int buffersize = 10000000;
    size_t buffer_bytes = buffersize*sizeof(double);
    double *memory=NULL;
    memory = (double *) malloc(2*buffer_bytes);
    //cudaMalloc((void **) &memory, 2*buffer_bytes);

    double *buffer=NULL;
    buffer = (double *) malloc(2*buffer_bytes);
    //cudaMalloc((void **)&buffer, 2*buffer_bytes);


    int rank, size, i;
    MPI_Datatype sendtype, type2, recvtype;

    MPI_Status status;
    double t_tv_start=0,t_tv_end=0,t_tv_total=0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size < 2)
    {
        printf("run with 2 processes.\n");
        MPI_Finalize();
        return 1;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int loop=0;
    for(loop=0; loop<60; loop++){
        t_tv_start = MPI_Wtime();
        MPI_Type_vector(buffersize,1,2,MPI_DOUBLE,&sendtype);
        MPI_Type_commit(&sendtype);
        MPI_Type_vector(buffersize,1,2,MPI_DOUBLE,&recvtype);
        MPI_Type_commit(&recvtype);

        if (rank == 0)
        {
                MPI_Send(memory, 1, sendtype, 1, 0, MPI_COMM_WORLD);
        }

        if (rank == 1)
        {

                MPI_Recv(buffer, 1, recvtype, 0, 0, MPI_COMM_WORLD, &status);
        }
        t_tv_total += (MPI_Wtime() - t_tv_start);
    }
    printf("time taken type_vector : %0.3f s rank : %d\n",  (t_tv_total), rank);
    MPI_Finalize();
    return 0;
}
