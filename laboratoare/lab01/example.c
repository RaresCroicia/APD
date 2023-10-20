#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// #define NUM_THREADS 2

void *f(void *arg) {
  long id = *(long *)arg;
  for (int i = 1; i <= 100; i++)
    printf("Hello World %d din thread-ul %ld!\n", i, id);
  pthread_exit(NULL);
}

void *g(void *arg) {
  int id = *(int *)arg;
  printf("Zarul tau a dat 6 %d!\n", id);
  pthread_exit(NULL);
}

void *h(void *arg) {
  float id = *(float *)arg;
  printf("MAGICIAN!!!! zaru tau a dat 2 %.2f!\n", id);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  // long NUM_THREADS = sysconf(_SC_NPROCESSORS_CONF);
  // pthread_t threads[NUM_THREADS];
  // int r;
  // long id;
  // void *status;
  // long ids[NUM_THREADS];

  // for (id = 0; id < NUM_THREADS; id++) {
  //   ids[id] = id;
  //   r = pthread_create(&threads[id], NULL, f, &ids[id]);

  //   if (r) {
  //     printf("Eroare la crearea thread-ului %ld\n", id);
  //     exit(-1);
  //   }
  // }

  // for (id = 0; id < NUM_THREADS; id++) {
  //   r = pthread_join(threads[id], &status);

  //   if (r) {
  //     printf("Eroare la asteptarea thread-ului %ld\n", id);
  //     exit(-1);
  //   }
  // }

  pthread_t threads[2];
  int r, zarInt = 6;
  float zarFloat = 2.75;
  void *status;
  r = pthread_create(&threads[0], NULL, g, &zarInt);
  if (r) {
    printf("Eroare la crearea thread-ului 0\n");
    exit(-1);
  }
  r = pthread_create(&threads[1], NULL, h, &zarFloat);
  if (r) {
    printf("Eroare la crearea thread-ului 1\n");
    exit(-1);
  }
  for (int i = 0; i < 2; i++) {
    r = pthread_join(threads[i], &status);
    if (r) {
      printf("Eroare la asteptarea thread-ului %d\n", i);
      exit(-1);
    }
  }

  return 0;
}
