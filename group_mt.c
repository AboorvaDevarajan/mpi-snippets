#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <mpi.h>
#define	THREAD_STACK_SIZE	80000

static int numthreads = 10;

void *
calluserfunc(void *args)
{
    int res;
    MPI_Comm the_comm;
    the_comm = MPI_COMM_WORLD;
    MPI_Group wgroup, group;
    MPI_Comm_group(the_comm, &wgroup);
    MPI_Group_union(wgroup, wgroup, &group);
    MPI_Group_compare(wgroup, group, &res);
    if (res != MPI_IDENT) {
        printf("not ident : %d\n", res);
        exit(1);
    }
	return 0;
}

void
td_c_twork()
{
    pthread_t 	ids[100];
	pthread_attr_t	attr;
	size_t		stacksize = 0;
	int		j, i, rtn;
	rtn = pthread_attr_init(&attr);
	if (rtn != 0) error("pthread_attr_init failed", rtn);
	rtn = pthread_attr_getstacksize(&attr, &stacksize);
	if (rtn != 0) error("pthread_attr_getstacksize failed", rtn);
	if (stacksize < THREAD_STACK_SIZE)
	    stacksize = THREAD_STACK_SIZE;
	rtn = pthread_attr_setstacksize(&attr, stacksize);
	if (rtn != 0) error("pthread_attr_getstacksize failed", rtn);
    for(i=1; i<=numthreads; i++) {
	    rtn = pthread_create(ids+i, &attr, calluserfunc, NULL);
	    if (rtn != 0 )error("pthread_create failed", rtn);
    }
	rtn = pthread_attr_destroy(&attr);
	if (rtn != 0) error("pthread_attr_destroy failed", rtn);
	(void) calluserfunc(NULL);
    for(i=1; i<=numthreads; i++) {
    	rtn = pthread_join(ids[i], (void**)0);
	    if (rtn != 0) error("pthread_join failed", rtn);
    }
}

int
main(int argc, char *argv[])
{
    int i, rank, numprocs, prov;
    int flag;
    MPI_Init_thread(&argc,&argv, MPI_THREAD_MULTIPLE, &prov);
    MPI_Is_thread_main(&flag);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    td_c_twork();
    MPI_Finalize();
    if (rank == 0) {
    printf(" passed\n");
    }
    exit(0);
}
