#include <iostream>
#include <vector>
#include <mpi.h>
#include <fstream>
#include <cstring>

using std::ofstream;
using namespace std;


/*
 * minimal test to demonstrate the issue in btl tcp issue.
 */

int main(int argc, char **argv) {
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm comm = MPI_COMM_WORLD;

    if (size != 4) {
        MPI_Finalize();
        return 1;
    }
    int len;
    
    MPI_Datatype i8space, i8i8;
    MPI_Type_size(MPI_LONG_LONG, &len);
    MPI_Type_create_resized(MPI_LONG_LONG, 0, 2*len, &i8space);
    printf("create resized : mpi_long_long, 0, %zu %p\n", 2*len, &i8space);
    MPI_Type_contiguous(2, MPI_LONG_LONG, &i8i8);
    MPI_Type_commit(&i8space);
    MPI_Type_commit(&i8i8);
    ofstream outdata;

    int *rcounts = new int[size];
    int *rdisps  = new int[size];
    long long *rbuf;
    
    if(rank == 0) {
        outdata.open("0.out"); 
        int buflen  = 23547840;
        rbuf = (long long *)malloc(buflen+1);
        memset(rbuf, 123, buflen);
        for(int i=0; i<buflen/8; i++)
        {
            rbuf[i] = 100*i;
        }
        rcounts[0] = 745126;
        rcounts[1] = 438904;
        rcounts[2] = 117366;
        rcounts[3] = 170344;
        
        
        rdisps[0] = 0;
        rdisps[1] = 745126;
        rdisps[2] = 1184030;
        rdisps[3] = 1301396;
        MPI_Barrier(MPI_COMM_WORLD);        
        MPI_Allgatherv(MPI_IN_PLACE,
                      1, MPI_INT,
                      rbuf, rcounts, rdisps, i8i8,
                      comm);
                      
        MPI_Barrier(MPI_COMM_WORLD);                      
       for(int i=0; i<buflen/8; i++)
       {
            outdata<< "rank : "<< rank << " index: "<< i <<" value: "<<rbuf[i]<<"\n";
       }
    }


    if(rank == 1) {
        outdata.open("1.out"); 
        int buflen  = 47095680;
        rbuf =  (long long *)malloc(buflen+1);
        memset(rbuf, 123, buflen);
        for(int i=0; i<buflen/8; i++)
        {
            rbuf[i] = 100*i;
        }
        
        rcounts[0] = 1490252;
        rcounts[1] = 877808;
        rcounts[2] = 234732;
        rcounts[3] = 340688;
        
        
        rdisps[0] = 0;
        rdisps[1] = 1490252;
        rdisps[2] = 2368060;
        rdisps[3] = 2602792;
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgatherv(MPI_IN_PLACE,
                      2, MPI_INT,
                      rbuf, rcounts, rdisps, i8space,
                      comm); 
        MPI_Barrier(MPI_COMM_WORLD);
        for(int i=0; i<buflen/8; i++)
        {
            outdata<< "rank : "<< rank << " index: "<< i <<" value: "<<rbuf[i]<<"\n";
        }   
    }
    
    
    if(rank == 2) {
        outdata.open("2.out"); 
        int buflen  = 47095680;
        rbuf =  (long long *)malloc(buflen+1);
        memset(rbuf, 123, buflen);
        for(int i=0; i<buflen/8; i++)
        {
            rbuf[i] = 100*i;
        }        
        rcounts[0] = 1490252;
        rcounts[1] = 877808;
        rcounts[2] = 234732;
        rcounts[3] = 340688;
        
        
        rdisps[0] = 0;
        rdisps[1] = 1490252;
        rdisps[2] = 2368060;
        rdisps[3] = 2602792;
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgatherv(MPI_IN_PLACE,
                      1, MPI_INT,
                      rbuf, rcounts, rdisps, i8space,
                      comm);   
        MPI_Barrier(MPI_COMM_WORLD);
        for(int i=0; i<buflen/8; i++)
        {
            outdata<< "rank : "<< rank << " index: "<< i <<" value: "<<rbuf[i]<<"\n";
        }
    }
    
    
    
    if(rank ==  3) {
        outdata.open("3.out"); 
        int buflen  = 23547840;
        rbuf =  (long long *)malloc(buflen+1);
        memset(rbuf, 123, buflen);
        for(int i=0; i<buflen/8; i++)
        {
            rbuf[i] = 100*i;
        }
        rcounts[0] = 745126;
        rcounts[1] = 438904;
        rcounts[2] = 117366;
        rcounts[3] = 170344;
        
        
        rdisps[0] = 0;
        rdisps[1] = 745126;
        rdisps[2] = 1184030;
        rdisps[3] = 1301396;
        MPI_Barrier(MPI_COMM_WORLD);        
        MPI_Allgatherv(MPI_IN_PLACE,
                      0, MPI_INT,
                      rbuf, rcounts, rdisps, i8i8,
                      comm);
        MPI_Barrier(MPI_COMM_WORLD);                      
       for(int i=0; i<buflen/8; i++)
       {
            outdata<< "rank : "<< rank << " index: "<< i <<" value: "<<rbuf[i]<<"\n";
       }
    }
    
    delete [] rdisps;
    delete [] rcounts;

    MPI_Finalize();
    return 0;
}


/*
[1,0]<stdout>:mpi_allgatherv : rbuf : 0x7fcc5698b010 sendcount : 1 sendtype : MPI_INT recvcount[0] : 745126 recvcount[1] : 438904 recvcount[2] : 117366 recvcount[3] : 170344 rdisps[0] : 0 rdisps[1] : 745126 rdisps[2] : 1184030 rdisps[3] : 1301396 rdt : 2
[1,0]<stdout>:pointer : 2

    [1,1]<stdout>:mpi_allgatherv : rbuf : 0x7fdd8fe0b010 sendcount : 2 sendtype : MPI_INT recvcount[0] : 1490252 recvcount[1] : 877808 recvcount[2] : 234732 recvcount[3] : 340688 rdisps[0] : 0 rdisps[1] : 1490252 rdisps[2] : 2368060 rdisps[3] : 2602792 rdt : 1
[1,1]<stdout>:pointer : 1

[1,2]<stdout>:mpi_allgatherv : rbuf : 0x7f5bf9316010 sendcount : 1 sendtype : MPI_INT recvcount[0] : 1490252 recvcount[1] : 877808 recvcount[2] : 234732 recvcount[3] : 340688 rdisps[0] : 0 rdisps[1] : 1490252 rdisps[2] : 2368060 rdisps[3] : 2602792 rdt : 1
[1,2]<stdout>:pointer : 1
[1,2]<stdout>:7004,2: buf[968292] is 11780264804, want 11780240472 (from 0)
[1,2]<stdout>:2: buf[968292] is 0x2be289364, want 2be283458 (from 0)
[1,2]<stdout>:abort: wrong data(2)

[1,3]<stdout>:mpi_allgatherv : rbuf : 0x18c2ea0 sendcount : 0 sendtype : MPI_INT recvcount[0] : 745126 recvcount[1] : 438904 recvcount[2] : 117366 recvcount[3] : 170344 rdisps[0] : 0 rdisps[1] : 745126 rdisps[2] : 1184030 rdisps[3] : 1301396 rdt : 2
[1,3]<stdout>:pointer : 2

0: 2   i8i8
1: 1   i8space
2: 1   i8space
3: 2   i8i8

[1,0]<stdout>:malloc : 23547840
[1,1]<stdout>:malloc : 47095680
[1,2]<stdout>:malloc : 47095680
[1,3]<stdout>:malloc : 23547840
*/
