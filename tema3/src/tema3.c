#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TRACKER_RANK 0
#define MAX_FILES 10
#define MAX_FILENAME 15
#define HASH_SIZE 33
#define MAX_CHUNKS 100

#define ACK 69420
#define NACK 42069
#define REQUEST_HASH 1000000
#define REQUEST_STATUS 1000001
#define QUIT_MESSAGE 1000002
#define REQUEST_SEEDS 1000003
#define RESPONSE_SEEDS 1000004
#define STATUS_MESSAGE 1000005
#define REQUEST_CHUNK 1000006

#define DOWNLOAD_TAG 0
#define UPLOAD_TAG 1
#define TRACKER_TAG 2

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

typedef struct {
    int no_files;
    char wanted_filenames[MAX_FILES][MAX_FILENAME];
} request_seeds_t;

typedef struct {
    int owner;
    file_info file;
} seed_file_t;

typedef struct {
    int no_files;
    seed_file_t files[MAX_FILES];
} response_seeds_t;

typedef struct {
    int type;
    union {
        request_seeds_t request_seeds;
        response_seeds_t response_seeds;
        int status;
        char chunk[HASH_SIZE];
    };
} message_t;

seed_file_t unownedFiles[MAX_FILES * 20];
int unownedFilesCount = 0;

file_info ownedFiles[MAX_FILES];
int ownedFilesCount = 0;

tracker_info trackerFiles[MAX_FILES];

char wantedFileNames[MAX_FILES][MAX_FILENAME];
int wantedFileNamesCount = 0;
bool waiting = false;

enum return_code readFiles(FILE* file) {
    fscanf(file, "%d", &ownedFilesCount);
    for(int i = 0; i < ownedFilesCount; i++) {
        fscanf(file, "%s", ownedFiles[i].filename);
        fscanf(file, "%d", &ownedFiles[i].no_chunks);
        for(int j = 0; j < ownedFiles[i].no_chunks; j++) {
            fscanf(file, "%s", ownedFiles[i].chunks[j]);
        }
    }
    fscanf(file, "%d", &wantedFileNamesCount);
    for(int i = 0; i < wantedFileNamesCount; i++) {
        fscanf(file, "%s", wantedFileNames[i]);
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
        printf("[TRACKER] %s\n", filename);
        MPI_Recv(&no_chunks, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, &status);
        printf("[TRACKER] %d\n", no_chunks);
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

    bool running = true;
    int hashes_received = 0;

    message_t message;
    message.type = REQUEST_SEEDS;
    message.request_seeds.no_files = wantedFileNamesCount;
    for(int i = 0; i < wantedFileNamesCount; i++) {
        strcpy(message.request_seeds.wanted_filenames[i], wantedFileNames[i]);
    }
    printf("[DOWNLOAD] Peer %d: trimit cerere de seed-uri catre tracker\n", rank);
    MPI_Send(&message, sizeof(message_t), MPI_BYTE, TRACKER_RANK, TRACKER_TAG, MPI_COMM_WORLD);
    printf("[DOWNLOAD] Peer %d: am trimis cerere de seed-uri catre tracker\n", rank);
    MPI_Recv(&message, sizeof(message_t), MPI_BYTE, TRACKER_RANK, TRACKER_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("[DOWNLOAD] Peer %d: am primit raspuns de la tracker\n", rank);
    printf("[DOWNLOAD] Peer %d: raspuns de tip %d\n", rank, message.type);
    printf("%d == %d\n", message.type, RESPONSE_SEEDS);
    if(message.type == RESPONSE_SEEDS) {
        for(int i = 0; i < message.response_seeds.no_files; i++) {
            unownedFiles[unownedFilesCount].owner = message.response_seeds.files[i].owner;
            unownedFiles[unownedFilesCount].file = message.response_seeds.files[i].file;
            unownedFilesCount++;
            printf("[DOWNLOAD] Peer %d: Seed-ul %d are %d chunkuri din fisierul %s\n", rank, message.response_seeds.files[i].owner, message.response_seeds.files[i].file.no_chunks, message.response_seeds.files[i].file.filename);
        }
    }

    while(running) {
        for(int i = 0; i < unownedFilesCount; i++) {
            if(unownedFiles[i].owner == rank)
                continue;
            // printf("Sunt pe aici\n");
            for(int j = 0; j < unownedFiles[i].file.no_chunks; j++) {
                message.type = REQUEST_CHUNK;
                strcpy(message.chunk, unownedFiles[i].file.chunks[j]);
                MPI_Send(&message, sizeof(message_t), MPI_BYTE, unownedFiles[i].owner, DOWNLOAD_TAG, MPI_COMM_WORLD);
                printf("[DOWNLOAD] Peer %d: am trimis cerere de chunk catre peer-ul %d\n", rank, unownedFiles[i].owner);
                MPI_Recv(&message, sizeof(message_t), MPI_BYTE, unownedFiles[i].owner, UPLOAD_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("[DOWNLOAD] Peer %d: am primit raspuns de tipul %d de la peer-ul %d\n", rank, message.type, unownedFiles[i].owner);
                if(message.type == STATUS_MESSAGE && message.status == ACK) {
                    bool found = false;
                    for(int k = 0; k < ownedFilesCount; k++) {
                        if(strcmp(ownedFiles[k].filename, unownedFiles[i].file.filename) == 0) {
                            printf("[DOWNLOAD] Peer %d: am primit chunkul %d din fisierul %s\n", rank, j, unownedFiles[i].file.filename);
                            strcpy(ownedFiles[k].chunks[ownedFiles[k].no_chunks], unownedFiles[i].file.chunks[j]);
                            ownedFiles[k].no_chunks++;
                            printf("[DOWNLOAD] Peer %d: am primit chunkul %d din fisierul %s\n", rank, j, unownedFiles[i].file.filename);
                            found = true;
                            break;
                        }
                    }
                    if(!found) {
                        strcpy(ownedFiles[ownedFilesCount].filename, unownedFiles[i].file.filename);
                        ownedFiles[ownedFilesCount].no_chunks = 1;
                        strcpy(ownedFiles[ownedFilesCount].chunks[0], unownedFiles[i].file.chunks[j]);
                        ownedFilesCount++;
                        printf("[DOWNLOAD] Peer %d: am primit chunkul %d din fisierul %s\n", rank, 0, unownedFiles[i].file.filename);
                    }
                    printf("[DOWNLOAD] Peer %d: Hash acceptat si primit\n", rank);
                    hashes_received++;
                }
            }
        }
    }


    return NULL;
}

void *upload_thread_func(void *arg)
{
    int rank = *(int*) arg;
    bool running = true;
    while(running) {
        message_t message;
        MPI_Status status;
        MPI_Recv(&message, sizeof(message_t), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        printf("[UPLOAD]: Peer %d: mesaj primit de la peer-ul %d\n", rank, status.MPI_SOURCE);
        printf("[UPLOAD]: Peer %d: mesaj de tip %d\n", rank, message.type);
        if(message.type == REQUEST_CHUNK) {
            printf("Peer %d: raspund cu chunkul cerut\n", rank);
            message.type = STATUS_MESSAGE;
            message.status = ACK;
            MPI_Send(&message, sizeof(message_t), MPI_BYTE, status.MPI_SOURCE, UPLOAD_TAG, MPI_COMM_WORLD);
        } else if(message.type == QUIT_MESSAGE) {
            printf("Peer %d: peer-ul %d s-a deconectat\n", rank, status.MPI_SOURCE);
            printf("Peer %d: trimitere confirmare catre peer-ul %d\n", rank, status.MPI_SOURCE);
            int confirm_message = ACK;
            MPI_Send(&confirm_message, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            printf("Peer %d: confirmare trimisa catre peer-ul %d\n", rank, status.MPI_SOURCE);
        }
    }
    return NULL;
}

void tracker(int numtasks, int rank) {
    bool running = true;
    
    for(int i = 1; i < numtasks; i++) {
        enum return_code code = receiveFromClient(i);
        if (code == ERROR) {
            printf("Eroare la primirea informatiilor de la clientul %d\n", i);
            exit(-1);
        }
        printf("[TRACKER] informatii primite de la clientul %d\n", i);
        for(int j = 0; j < trackerFiles[i].no_files; j++) {
            printf("[TRACKER] %s\n", trackerFiles[i].files[j].filename);
        }
    }
    printf("[TRACKER] informatii primite de la toti clientii\n");
    printf("[TRACKER] trimitere confirmare catre toti clientii\n");
    int confirm_message = ACK;
    for(int i = 1; i < numtasks; i++) {
        MPI_Send(&confirm_message, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    printf("[TRACKER] confirmare trimisa catre toti clientii\n");

    while(running) {
        message_t message;
        MPI_Status status;
        MPI_Recv(&message, sizeof(message_t), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        printf("[TRACKER] mesaj primit de la clientul %d\n", status.MPI_SOURCE);
        printf("[TRACKER] mesaj de tip %d\n", message.type);
        if(message.type == REQUEST_SEEDS) {
            printf("[TRACKER] raspund cu informatiile despre fisierele cerute\n");
            int no_files = message.request_seeds.no_files;
            message.type = RESPONSE_SEEDS;
            message.response_seeds.no_files = 0;
            for(int i = 0; i < no_files; i++) {
                for(int j = 1; j < numtasks; j++) {
                    for(int k = 0; k < trackerFiles[j].no_files; k++) {
                        if(strcmp(message.request_seeds.wanted_filenames[i], trackerFiles[j].files[k].filename) == 0) {
                            message.response_seeds.files[message.response_seeds.no_files].owner = j;
                            message.response_seeds.files[message.response_seeds.no_files].file = trackerFiles[j].files[k];
                            message.response_seeds.no_files++;
                        }
                    }
                }
            }
            MPI_Send(&message, sizeof(message_t), MPI_BYTE, status.MPI_SOURCE, TRACKER_TAG, MPI_COMM_WORLD);
            printf("[TRACKER] raspuns trimis catre clientul %d cu tipul %s\n", status.MPI_SOURCE, message.type == RESPONSE_SEEDS ? "RESPONSE_SEEDS" : "ERROR"); 
        } else if(message.type == QUIT_MESSAGE) {
            printf("[TRACKER] clientul %d s-a deconectat\n", status.MPI_SOURCE);
            printf("[TRACKER] trimitere confirmare catre clientul %d\n", status.MPI_SOURCE);
            confirm_message = ACK;
            MPI_Send(&confirm_message, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            printf("[TRACKER] confirmare trimisa catre clientul %d\n", status.MPI_SOURCE);
        }
    }
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
