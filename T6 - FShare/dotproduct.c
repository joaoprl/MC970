/* 
 * dot_product.c - A parallel program to perform dot product of two vectors
 */

//	compile as: gcc dotproduct.c -lpthread -o dotproduct
//	run as: ./dotproduct <n> <num_threads>

#include <pthread.h>	
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <limits.h>

#define MAXTHREADS	8
#define	STRIDE	64

void *sum(void *p);

// global shared variables
double *v1, *v2;
double sumtotal = 0.0;
int numthreads, n;
pthread_mutex_t mutex;

int main(int argc, char **argv) {
  pthread_t tid[MAXTHREADS];
  int i, myid[MAXTHREADS];
  struct timeval start, end;
	
  gettimeofday(&start, NULL); /* start timing */
  pthread_mutex_init(&mutex, NULL);
		
  scanf("%d %d", &n, &numthreads);

  v1 = (double *) malloc(n * sizeof (double));
  v2 = (double *) malloc(n * sizeof (double));
  for (i = 0; i < n; i++) {
    v1[i] = 1;
    v2[i] = 2; 
  }
	
  for (i = 0; i < numthreads; i++) {
    myid[i] = i;
    pthread_create(&tid[i], NULL, sum, &myid[i]);
  }
  for (i = 0; i < numthreads; i++) {
    pthread_join(tid[i], NULL);
  }

  free(v1); free(v2);
  pthread_mutex_destroy(&mutex);
  gettimeofday(&end, NULL); /* end timing */
  long spent = (end.tv_sec * 1000000 + end.tv_usec)
    - (start.tv_sec * 1000000 + start.tv_usec);

  printf("%.0f\n%ld\n", sumtotal, spent);
  return 0;
}
//-----------------------------------------------
void *sum(void *p) {
  int myid = *((int *) p);
  int start = (myid * (long long) n) / numthreads;
  int end = ((myid + 1) * (long long) n) / numthreads;
  int i, k = myid, m = 1, len;
  double sum = 0.0;
  len = end - start;
  
  for (i = 0; i < len / STRIDE; i++) {	
    for (k = 0; k < STRIDE; k++) {	
      m = start + (i * STRIDE + k);
      sum += v1[m] * v2[m];
    }
  }
	
  if ((len / STRIDE) * STRIDE != len) {	// do the remainder, if any
    for (m = start + (len / STRIDE) * STRIDE; m < end; m++) { 
      sum += v1[m] * v2[m];		
    }
  }
	
  pthread_mutex_lock(&mutex);
  sumtotal += sum;
  pthread_mutex_unlock(&mutex);
	
  return NULL;
}


