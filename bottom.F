        program bottom02
        include "mpif.h"

c       ---parameters---

c       ---variables---
        integer me, tasks, ii, len, numtasks, myself,ierror
        integer disp, type, newtype
        integer (kind=MPI_ADDRESS_KIND) adisp
        integer status(MPI_STATUS_SIZE)
        common ii

c       ---enlist in mpi---
        call MPI_Init(ierror)
        call MPI_Comm_size(MPI_COMM_WORLD,numtasks,ierror)
        call MPI_Comm_rank(MPI_COMM_WORLD,me,ierror)
        call MPI_Comm_rank(MPI_COMM_WORLD,myself,ierror)

        len=1
        call MPI_Address(ii,disp,ierror)
        write(* , *) disp
        type=MPI_INTEGER
        call MPI_Type_struct(1,len,disp,type,newtype,ierror)
        call MPI_Type_commit(newtype,ierror)

        if (myself.eq.0) then
          ii=2
          call MPI_Send(MPI_BOTTOM,1,newtype,1,0,MPI_COMM_WORLD,ierror)
        else if (myself.eq.1) then
          ii=0
          call MPI_Recv(MPI_BOTTOM,1,newtype,0,0,
     +                  MPI_COMM_WORLD,status,ierror)
        endif
        call MPI_Type_free(newtype, ierror)
 200    call MPI_Finalize(ierror)

        stop
        end
