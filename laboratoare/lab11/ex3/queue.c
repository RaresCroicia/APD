#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>

typedef struct {
    int size;
    int arr[1000];
} queue;

int main (int argc, char *argv[]) {
    int numtasks, rank;

    queue q;
    // TODO: declare the types and arrays for offsets and block counts

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Datatype queue_type;
    MPI_Datatype type[2] = {MPI_INT, MPI_INT};
    int blocklen[2] = {1, 1000};
    MPI_Aint offsets[2];

    offsets[0] = offsetof(queue, size);
    offsets[1] = offsetof(queue, arr);

    MPI_Type_create_struct(2, blocklen, offsets, type, &queue_type);
    MPI_Type_commit(&queue_type);

    // TODO: create the MPI data type, using offsets and block counts, and commit the data type
 
    // First process starts the circle.
    if (rank == 0) {
        // First process starts the circle.
        // Generate a random number and add it in queue.
        // Sends the queue to the next process.
        q.size = 0;
        srand(30);
        q.arr[q.size++] = rand() % 100;
        // MPI_Send(&q, sizeof(queue), MPI_BYTE, 1, 0, MPI_COMM_WORLD);
        // MPI_Recv(&q, sizeof(queue), MPI_BYTE, numtasks - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&q, 1, queue_type, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&q, 1, queue_type, numtasks - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int i = 0; i < q.size; i++) {
            printf("%d ", q.arr[i]);
        }

    } else if (rank == numtasks - 1) {
        // Last process close the circle.
        // Receives the queue from the previous process.
        // Generate a random number and add it in queue.
        // Sends the queue to the first process.
        // MPI_Recv(&q, sizeof(queue), MPI_BYTE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&q, 1, queue_type, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        srand(42);
        q.arr[q.size++] = rand() % 100;
        // MPI_Send(&q, sizeof(queue), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&q, 1, queue_type, 0, 0, MPI_COMM_WORLD);
    } else {
        // Middle process.
        // Receives the queue from the previous process.
        // Generate a random number and add it in queue.
        // Sends the queue to the next process.
        // MPI_Recv(&q, sizeof(queue), MPI_BYTE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&q, 1, queue_type, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        srand(49);
        q.arr[q.size++] = rand() % 100;
        // MPI_Send(&q, sizeof(queue), MPI_BYTE, rank + 1, 0, MPI_COMM_WORLD);
        MPI_Send(&q, 1, queue_type, rank + 1, 0, MPI_COMM_WORLD);
    }

    // TODO: Process 0 prints the elements from queue
    
    // TODO: free the newly created MPI data type

    MPI_Finalize();
}