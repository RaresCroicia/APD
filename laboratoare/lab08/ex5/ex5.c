#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROOT 0

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    // Checks the number of processes allowed.
    if (numtasks != 2) {
        printf("Wrong number of processes. Only 2 allowed!\n");
        MPI_Finalize();
        return 0;
    }

    // How many numbers will be sent.
    int send_numbers = 10;

    if (rank == 0) {
        srand(time(NULL));
        int *numbers = (int *)malloc(send_numbers * sizeof(int));
        int *tags = (int *)malloc(send_numbers * sizeof(int));
        for(int i = 0; i < send_numbers; i++) {
            numbers[i] = rand() % 100;
            tags[i] = rand() % 100;
        }
        // Generate the random numbers.
        // Generate the random tags.
        // Sends the numbers with the tags to the second process.
        for(int i = 0; i < send_numbers; i++) {
            printf("Process [%d] sends %d with tag %d.\n", rank, numbers[i], tags[i]);
            MPI_Send(&numbers[i], 1, MPI_INT, 1, tags[i], MPI_COMM_WORLD);
        }
    } else {

        for(int i = 0; i < send_numbers; i++) {
            MPI_Status status;
            int value;
            int tag;
            // Receives the numbers with the tags from the first process.
            // Prints the numbers with their corresponding tags.
            MPI_Recv(&value, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            tag = status.MPI_TAG;
            printf("Process [%d] received %d with tag %d.\n", rank, value, tag);
        }
        // Receives the information from the first process.
        // Prints the numbers with their corresponding tags.

    }

    MPI_Finalize();

}

