
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <mpi.h>
#define	THREAD_STACK_SIZE	80000
static MPI_Comm t_comms[1000];
static MPI_Comm op_comms[1000];
static int reps = 100;
static int numtids = 2;

static 	int 		ticket = 0;
static 	volatile int 	brelease = 0;
static 	volatile int 	bcount = 0;
void *(*userfunc)(void*);
static volatile int pids[100], killnpids=0;
static  pthread_mutex_t mt_block = PTHREAD_MUTEX_INITIALIZER;

void *
calluserfunc(void *args)
{
	int mytid;
	mytid = ((int*)args)[0];

	pids[mytid] = getpid();
	td_barrier();
	if (mytid==0) { killnpids = numtids; }

	td_barrier();
	userfunc(args);
	return 0;
}


void
td_barrier() {

        pthread_mutex_lock(&mt_block);
        bcount++;
        if (bcount == numtids) brelease = 1;
        pthread_mutex_unlock(&mt_block);

        while (!brelease) {
		sched_yield();
        }


        pthread_mutex_lock(&mt_block);
        bcount--;
        if (bcount == 0) brelease = 0;
        pthread_mutex_unlock(&mt_block);

        while (brelease) {
		sched_yield();
        }
}

void
td_c_twork(int num, void *(*func1)(void *))

{

    pthread_t 	ids[100];
	pthread_attr_t	attr;
	size_t		stacksize = 0;

	int 	 	myinfo[1000];
	int		j, i, rtn;

/*
 * Set the thread stack size to THREAD_STACK_SIZE bytes.
 */
    	rtn = pthread_attr_init(&attr);
    	if (rtn != 0) error("pthread_attr_init failed", rtn);

    	rtn = pthread_attr_getstacksize(&attr, &stacksize);
    	if (rtn != 0) error("pthread_attr_getstacksize failed", rtn);

    	if (stacksize < THREAD_STACK_SIZE)
		stacksize = THREAD_STACK_SIZE;

    	rtn = pthread_attr_setstacksize(&attr, stacksize);
    	if (rtn != 0) error("pthread_attr_getstacksize failed", rtn);


    	numtids = num;

    	for (j = 0, i = 0; i < num; j +=2, i++) {
		myinfo[j] = i;
		myinfo[j+1] = num;
    	}

	    userfunc = func1;
    	for (j = 2, i = 1; i < num; j +=2, i++) {
		    myinfo[j] = i;
		    myinfo[j+1] = num;
        	rtn = pthread_create(ids+i, &attr, calluserfunc, myinfo+j);
        	if (rtn != 0 )error("pthread_create failed", rtn);

    	}


    	rtn = pthread_attr_destroy(&attr);
    	if (rtn != 0) error("pthread_attr_destroy failed", rtn);



    	(void) calluserfunc(myinfo);


    	for (i = 1; i < num; i++) {
		rtn = pthread_join(ids[i], (void**)0);
		if (rtn != 0) error("pthread_join failed", rtn);
    	}
	killnpids = 0;
}
static void
doit(void *args)
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
        }
    td_barrier();
}


int
main(int argc, char *argv[])
{
    int zero = 0;
    int i, rank, numprocs, prov;
    int flag;
    MPI_Init_thread(&argc,&argv, MPI_THREAD_MULTIPLE, &prov);
    MPI_Is_thread_main(&flag);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    td_c_twork(numtids, doit);
    MPI_Finalize();
    if (rank == 0) {
    printf(" passed\n");
    }
    exit(0);
}


