#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    int numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Request request;
    MPI_Status status;
    int flag;

    int recv_num;

    // First process starts the circle.
    if (rank == 0) {
        // First process starts the circle.
        // Generate a random number.
        // Send the number to the next process.

        srand(time(NULL));
        int num = rand() % 100;
        printf("Process %d generated number %d\n", rank, num);
        MPI_Isend(&num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);

        if(flag) {
            printf("Process %d sent number %d\n", rank, num);
        } else {
            printf("Process %d didn't send %d yet.\n", rank, num);
            MPI_Wait(&request, &status);
        }

        MPI_Irecv(&recv_num, 1, MPI_INT, numtasks - 1, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);

        if(flag) {
            printf("Process %d received number %d\n", rank, recv_num);
        } else {
            printf("Process %d didn't receive number yet.\n", rank);
            MPI_Wait(&request, &status);
        }

        printf("Process %d received number %d eventually.\n", rank, recv_num);

    } else if (rank == numtasks - 1) {
        // Last process close the circle.
        // Receives the number from the previous process.
        // Increments the number.
        // Sends the number to the first process.

        MPI_Irecv(&recv_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);

        if(flag) {
            printf("Process %d received number %d\n", rank, recv_num);
        } else {
            printf("Process %d didn't receive number yet.\n", rank);
            MPI_Wait(&request, &status);
        }

        MPI_Isend(&recv_num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);

        if(flag) {
            printf("Process %d sent number %d\n", rank, recv_num);
        } else {
            printf("Process %d didn't send number yet.\n", rank);
            MPI_Wait(&request, &status);
        }

        printf("Process %d received number %d eventually.\n", rank, recv_num);

    } else {
        // Middle process.
        // Receives the number from the previous process.
        // Increments the number.
        // Sends the number to the next process.

        MPI_Irecv(&recv_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);

        if(flag) {
            printf("Process %d received number %d\n", rank, recv_num);
        } else {
            printf("Process %d didn't receive %d yet.\n", rank, recv_num);
            MPI_Wait(&request, &status);
        }

        MPI_Isend(&recv_num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);

        if(flag) {
            printf("Process %d sent number %d\n", rank, recv_num);
        } else {
            printf("Process %d didn't send %d yet.\n", rank, recv_num);
            MPI_Wait(&request, &status);
        }

        printf("Process %d received number %d eventually.\n", rank, recv_num);

    }

    MPI_Finalize();

}

