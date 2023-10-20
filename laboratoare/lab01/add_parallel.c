#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define min(X,Y) (((X) < (Y)) ? (X) : (Y))
/*
    schelet pentru exercitiul 5
*/

typedef struct {
  int start;
  int end;
} thread_arg_t;

int *arr;
int array_size;
int num_threads;

void *f(void *arg) {
  thread_arg_t *thread_arg = (thread_arg_t *)arg;
  for (int i = thread_arg->start; i < thread_arg->end; i++) {
    arr[i] += 100;
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Specificati dimensiunea array-ului si numarul de thread-uri\n");
    exit(-1);
  }
  struct timespec start, finish; 
  double elapsed; 

  array_size = atoi(argv[1]);
  num_threads = atoi(argv[2]);

  arr = malloc(array_size * sizeof(int));
  for (int i = 0; i < array_size; i++) {
    arr[i] = i;
  }

  // for (int i = 0; i < array_size; i++) {
  //   printf("%d", arr[i]);
  //   if (i != array_size - 1) {
  //     printf(" ");
  //   } else {
  //     printf("\n");
  //   }
  // }

  pthread_t threads[num_threads];
  int r;
  void *status;

  clock_gettime(CLOCK_MONOTONIC, &start);

  for(int i = 0; i < num_threads; i++) {
    thread_arg_t *arg = malloc(sizeof(thread_arg_t));
    arg->start = i * (double) array_size / num_threads;
    arg->end = min((i + 1) * (double)array_size / num_threads, array_size);
    r = pthread_create(&threads[i], NULL, f, (void*)arg);
    if (r) {
      printf("Eroare la crearea thread-ului %d\n", i);
      exit(-1);
    }
    free(arg);
  }

  for (int i = 0; i < num_threads; i++) {
    r = pthread_join(threads[i], &status);
    if (r) {
      printf("Eroare la asteptarea thread-ului %d\n", i);
      exit(-1);
    }
  }
  
  clock_gettime(CLOCK_MONOTONIC, &finish);

  elapsed = (finish.tv_sec - start.tv_sec);
  elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

  printf("Timpul de executie paralel: %lf\n", elapsed);

  // for (int i = 0; i < array_size; i++) {
  //   printf("%d", arr[i]);
  //   if (i != array_size - 1) {
  //     printf(" ");
  //   } else {
  //     printf("\n");
  //   }
  // }

  return 0;
}
