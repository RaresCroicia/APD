#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRACKER_RANK 0
#define MAX_FILES 10
#define MAX_FILENAME 15
#define HASH_SIZE 32
#define MAX_CHUNKS 100

#define ACK 69420

const char* getFileName(int rank) {
    static char filename[MAX_FILENAME];
    sprintf(filename, "in%d.txt", rank);
    return filename;
}

enum return_code {
    SUCCESS,
    ERROR
};

typedef struct {
    char filename[MAX_FILENAME];
    int no_chunks;
    char chunks[MAX_CHUNKS][HASH_SIZE];
} file_info;

typedef struct {
    file_info files[MAX_FILES];
    int no_files;
} tracker_info;

file_info ownedFiles[MAX_FILES];
tracker_info trackerFiles[MAX_FILES];
char wantedFiles[MAX_FILES][MAX_FILENAME];
int wantedFilesCount = 0;
int ownedFilesCount = 0;

enum return_code readFiles(FILE* file) {
    fscanf(file, "%d", &ownedFilesCount);
    for(int i = 0; i < ownedFilesCount; i++) {
        fscanf(file, "%s", ownedFiles[i].filename);
        fscanf(file, "%d", &ownedFiles[i].no_chunks);
        for(int j = 0; j < ownedFiles[i].no_chunks; j++) {
            fscanf(file, "%s", ownedFiles[i].chunks[j]);
        }
    }
    fscanf(file, "%d", &wantedFilesCount);
    for(int i = 0; i < wantedFilesCount; i++) {
        fscanf(file, "%s", wantedFiles[i]);
    }
    return SUCCESS;
}

enum return_code sendToTracker() {
    MPI_Send(&ownedFilesCount, 1, MPI_INT, TRACKER_RANK, 0, MPI_COMM_WORLD);
    for(int i = 0; i < ownedFilesCount; i++) {
        MPI_Send(ownedFiles[i].filename, strlen(ownedFiles[i].filename), MPI_CHAR, TRACKER_RANK, 0, MPI_COMM_WORLD);
        printf("Sent: %s\n", ownedFiles[i].filename);
        MPI_Send(&ownedFiles[i].no_chunks, 1, MPI_INT, TRACKER_RANK, 0, MPI_COMM_WORLD);
        printf("Sent: %d\n", ownedFiles[i].no_chunks);
        for(int j = 0; j < ownedFiles[i].no_chunks; j++) {
            MPI_Send(ownedFiles[i].chunks[j], HASH_SIZE, MPI_CHAR, TRACKER_RANK, 0, MPI_COMM_WORLD);
            // printf("Sent: %s\n", ownedFiles[i].chunks[j]);
        }
    }
    return SUCCESS;
}

enum return_code receiveFromClient(int rank) {
    MPI_Status status;
    int no_files;
    char filename[MAX_FILENAME];
    int no_chunks;
    char chunks[MAX_CHUNKS][HASH_SIZE];

    MPI_Recv(&no_files, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, &status);
    for(int i = 0; i < no_files; i++) {
        MPI_Recv(filename, MAX_FILENAME, MPI_CHAR, rank, 0, MPI_COMM_WORLD, &status);
        printf("Tracker: %s\n", filename);
        MPI_Recv(&no_chunks, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, &status);
        printf("Tracker: %d\n", no_chunks);
        for(int j = 0; j < no_chunks; j++) {
            MPI_Recv(chunks[j], HASH_SIZE, MPI_CHAR, rank, 0, MPI_COMM_WORLD, &status);
        }
        strcpy(trackerFiles[rank].files[i].filename, filename);
        trackerFiles[rank].files[i].no_chunks = no_chunks;
        for(int j = 0; j < no_chunks; j++) {
            strcpy(trackerFiles[rank].files[i].chunks[j], chunks[j]);
        }
    }
    trackerFiles[rank].no_files = no_files;
    return SUCCESS;    
}


void *download_thread_func(void *arg)
{
    int rank = *(int*) arg;

    return NULL;
}

void *upload_thread_func(void *arg)
{
    int rank = *(int*) arg;

    return NULL;
}

void tracker(int numtasks, int rank) {
    for(int i = 1; i < numtasks; i++) {
        enum return_code code = receiveFromClient(i);
        if (code == ERROR) {
            printf("Eroare la primirea informatiilor de la clientul %d\n", i);
            exit(-1);
        }
        printf("Tracker: informatii primite de la clientul %d\n", i);
        for(int j = 0; j < trackerFiles[i].no_files; j++) {
            printf("Tracker: %s\n", trackerFiles[i].files[j].filename);
        }
    }
    printf("Tracker: informatii primite de la toti clientii\n");
    printf("Tracker: trimitere confirmare catre toti clientii\n");
    int confirm_message = ACK;
    for(int i = 1; i < numtasks; i++) {
        MPI_Send(&confirm_message, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    printf("Tracker: confirmare trimisa catre toti clientii\n");
}

void peer(int numtasks, int rank) {
    pthread_t download_thread;
    pthread_t upload_thread;
    void *status;
    int r;

    FILE* file = fopen(getFileName(rank), "r");
    if (file == NULL) {
        printf("Eroare la deschiderea fisierului\n");
        exit(-1);
    }
  
    enum return_code code = readFiles(file);
    if (code == ERROR) {
        printf("Eroare la citirea fisierului\n");
        exit(-1);
    }
    fclose(file);
    printf("Peer %d: %d fisiere detinute\n", rank, ownedFilesCount);

    code = sendToTracker();
    if (code == ERROR) {
        printf("Eroare la trimiterea informatiilor catre tracker\n");
        exit(-1);
    }
    printf("Peer %d: informatii trimise catre tracker\n", rank);
    printf("Waiting for tracker to confirm...\n");
    int confirm_message;
    MPI_Recv(&confirm_message, 1, MPI_INT, TRACKER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if(confirm_message != ACK) {
        printf("Tracker-ul nu a confirmat sau a confirmat gresit!\n");
        exit(-1);
    }
    printf("Peer %d: Tracker-ul a confirmat!\n", rank);

    r = pthread_create(&download_thread, NULL, download_thread_func, (void *) &rank);
    if (r) {
        printf("Eroare la crearea thread-ului de download\n");
        exit(-1);
    }

    r = pthread_create(&upload_thread, NULL, upload_thread_func, (void *) &rank);
    if (r) {
        printf("Eroare la crearea thread-ului de upload\n");
        exit(-1);
    }

    r = pthread_join(download_thread, &status);
    if (r) {
        printf("Eroare la asteptarea thread-ului de download\n");
        exit(-1);
    }

    r = pthread_join(upload_thread, &status);
    if (r) {
        printf("Eroare la asteptarea thread-ului de upload\n");
        exit(-1);
    }
}
 
int main (int argc, char *argv[]) {
    int numtasks, rank;
    
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    
    if (provided < MPI_THREAD_MULTIPLE) {
        fprintf(stderr, "MPI nu are suport pentru multi-threading\n");
        exit(-1);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == TRACKER_RANK) {
        tracker(numtasks, rank);
    } else {
        peer(numtasks, rank);
    }

    MPI_Finalize();
}
