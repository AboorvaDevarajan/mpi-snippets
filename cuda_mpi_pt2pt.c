#include<mpi.h>
#include<cuda.h>
#include<cuda_runtime_api.h>
#include<stdio.h>
#define MAX_MSG_SIZE (1<<22)
#define MYBUFSIZE (MAX_MSG_SIZE)
#include <sys/types.h>
#include <sys/syscall.h>

int
main (int argc, char *argv[])
{
    int myid, numprocs, i;
    int size;
    MPI_Status reqstat;
    char *s_buf, *r_buf;
    double t_start = 0.0, t_end = 0.0;
    char *str= NULL;
    int local_rank = -1, dev_count = 0;
    int dev_id = 0;

    pid_t x = syscall(__NR_gettid);

    CUresult curesult = CUDA_SUCCESS;
    CUdevice cuDevice;
    CUcontext cuContext;

    str = getenv("OMPI_COMM_WORLD_LOCAL_RANK");
    local_rank = atoi(str);
    printf("\n local_rank : %d %d\n", local_rank, x);

    if (local_rank >= 0) {
        curesult = cudaGetDeviceCount(&dev_count);
        printf("\n device count : %d\n", dev_count);
        if (curesult != CUDA_SUCCESS) {
            return 1;
        }
        dev_id = local_rank % dev_count;
    }

    curesult = cuInit(0);
    if (curesult != CUDA_SUCCESS) {
        return 1;
    }

    curesult = cuDeviceGet(&cuDevice, dev_id);
    if (curesult != CUDA_SUCCESS) {
        return 1;
    }

    printf("\ncreating cuda context");
    curesult = cuCtxCreate(&cuContext, 0, cuDevice);
    if (curesult != CUDA_SUCCESS) {
        return 1;
    }
    printf("\ncreated cuda context : %p rank : %d\n" , cuContext, local_rank );

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);


    cudaError_t cuerr = cudaSuccess;

    cuerr = cudaMalloc((void **)&s_buf, MYBUFSIZE);

    if (cudaSuccess != cuerr) {
        fprintf(stderr, "Could not allocate device memory\n");
        return 1;
    }
    cuerr = cudaMalloc((void **)&r_buf, MYBUFSIZE);
    printf("\n send_buf : %p recv_buf : %p\n", s_buf, r_buf);

    if (cudaSuccess != cuerr) {
        fprintf(stderr, "Could not allocate device memory\n");
        return 1;
    }

    
    for(size = 0; size <= MAX_MSG_SIZE; size = (size ? size * 2 : 1)) {
	
	printf("size : %d\n", size);
        MPI_Barrier(MPI_COMM_WORLD);

        if(myid == 0) {
                MPI_Send(s_buf, size, MPI_CHAR, 1, 1, MPI_COMM_WORLD);
                MPI_Recv(r_buf, size, MPI_CHAR, 1, 1, MPI_COMM_WORLD, &reqstat);
        }

        else if(myid == 1) {
                MPI_Recv(r_buf, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &reqstat);
                MPI_Send(s_buf, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}

