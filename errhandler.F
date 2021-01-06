#include "foptions.h"

      INTEGER FUNCTION ERR1(COMMIN, CODE)

      INCLUDE 'externalf.h'
      INCLUDE 'mpif.h'

      INTEGER COMMIN
      INTEGER CODE
      INTEGER IERR
      INTEGER PASS
      INTEGER FAIL
      INTEGER COUNT1
      INTEGER COUNT2
      INTEGER COMM
      COMMON /ERRCOM/PASS, FAIL, COUNT1, COUNT2, COMM
      CHARACTER*(MPI_MAX_OBJECT_NAME) :: comm_name
      INTEGER       :: rlen
      INTEGER CLASS
C     Prevent compiler warning
      ERR1 = MPI_SUCCESS
C
C   Handles errors for MPI_COMM_WORLD
C
      IF (COUNT1 .EQ. 0) THEN
C         Bad length on MPI_SEND
        CALL MPI_ERROR_CLASS(CODE, CLASS, IERR)
        IF (CLASS .NE. MPI_ERR_COUNT) THEN         
          FAIL = FAIL + 1
        ELSE
         PASS = PASS + 1
        END IF
        IF (COMMIN .NE. MPI_COMM_WORLD) THEN          
          FAIL = FAIL + 1
        END IF
        CALL MPI_COMM_GET_NAME(COMMIN, comm_name, rlen, ierr)
        print *, "name 1: ", comm_name, ":"
      ELSE IF (COUNT1 .EQ. 1) THEN
C         Bad communicator in MPI_BARRIER
        CALL MPI_ERROR_CLASS(CODE, CLASS, IERR)
        IF (CLASS .NE. MPI_ERR_COMM) THEN          
          FAIL = FAIL + 1
        ELSE
         PASS = PASS + 1
        END IF
        IF (COMMIN .NE. MPI_COMM_WORLD) THEN
          CALL MPI_COMM_GET_NAME(COMMIN, comm_name, rlen, ierr)
          print *, "name 2: ", comm_name, ":"
          FAIL = FAIL + 1
        END IF
      ELSE        
        FAIL = FAIL + 1
      END IF
      COUNT1 = COUNT1 + 1
      END
      PROGRAM MAIN

#include "mpitestf.h"
      INCLUDE 'externalf.h'
      INCLUDE 'mpif.h'

      EXTERNAL ERR1
      EXTERNAL ERR2
      INTEGER PASS
      INTEGER FAIL              
      INTEGER COUNT1
      INTEGER COUNT2
      INTEGER COMM
      COMMON /ERRCOM/PASS, FAIL, COUNT1, COUNT2, COMM
      INTEGER ERRH1
      INTEGER IERR
      INTEGER ERR
      INTEGER ERRSIZE 

      CALL MPI_INIT(IERR)
      IF (IERR .NE. MPI_SUCCESS) THEN

      END IF
      PASS = 0
      FAIL = 0

C
C Do a quick/safe OMPI test to see if we're checking MPI
C parameters or not
C
      CALL MPI_COMM_DUP(MPI_COMM_WORLD, COMM, IERR)
      IF (IERR .NE. MPI_SUCCESS) THEN
        CALL MPI_ERROR_STRING(IERR, '', ERRSIZE, ERR)
      END IF
      CALL MPI_ERRHANDLER_CREATE(ERR1, ERRH1, IERR)
      IF (IERR .NE. MPI_SUCCESS) THEN
        CALL MPI_ERROR_STRING(IERR, '', ERRSIZE, ERR)
      END IF
      PASS = PASS + 1
      CALL MPI_ERRHANDLER_SET(MPI_COMM_WORLD, ERRH1, IERR)
      IF (IERR .NE. MPI_SUCCESS) THEN
        CALL MPI_ERROR_STRING(IERR, '', ERRSIZE, ERR)
      END IF
      PASS = PASS + 1
      CALL MPI_ERRHANDLER_SET(MPI_COMM_SELF, ERRH1, IERR)
      IF (IERR .NE. MPI_SUCCESS) THEN
        CALL MPI_ERROR_STRING(IERR, '', ERRSIZE, ERR)
      END IF
      PASS = PASS + 1
      CALL MPI_ERRHANDLER_FREE(ERRH1, IERR)
      IF (IERR .NE. MPI_SUCCESS) THEN
        CALL MPI_ERROR_STRING(IERR, '', ERRSIZE, ERR)
      ELSE
        PASS = PASS + 1
      END IF
C
C   Now create some errors and verify that they work
C
C      MPI_COMM_WORLD
      CALL MPI_SEND(IERR, -1, MPI_INTEGER, 0, 0, MPI_COMM_WORLD, IERR)
C      This should be handled by MPI_COMM_SELF
      CALL MPI_BARRIER(MPI_COMM_NULL, IERR)
      IF (COUNT1 .NE. 2) THEN
        FAIL = FAIL + 1
      END IF
      CALL MPI_FINALIZE(IERR)
      IF (FAIL .EQ. 77) THEN
          FAIL = FAIL + 1
      END IF
      CALL EXIT(FAIL)
      END
