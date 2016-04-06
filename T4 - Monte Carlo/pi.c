#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct{
  unsigned int n;
  long long unsigned int hits;
}Args;

/* thread throws args->n darts and save in args->hits how many hits */
void *throw(void *args)
{
  Args * argsS = (Args*)args;

  long long unsigned int in = 0, i;
  double x, y, d;
  unsigned int seed = time(NULL);
  
  for (i = 0; i < argsS->n; i++) {
    x = ((rand_r(&seed) % 1000000)/500000.0)-1;
    y = ((rand_r(&seed) % 1000000)/500000.0)-1;
    d = ((x*x) + (y*y));
    if (d <= 1) in+=1;
  }
  argsS->hits = in;

  return NULL;
}

long long unsigned int monte_carlo_pi_parallel(unsigned int n, int nt) {
  long long unsigned int in;
  int i;

  pthread_t threads[nt];
  Args args[nt];
  for(i = 0; i < nt; i++){
    args[i].n = n / nt;
    if(pthread_create(&(threads[i]), NULL, throw, (void*)&(args[i]))){ // throws a new dart
      printf("Error creating thread\n");
      exit(1);
    }
  }

  in = 0;
  for(i = 0; i < nt; i++){
    pthread_join(threads[i], NULL);
    in += args[i].hits; // add hits
  }

  return in;
}

int main(void) {
  int size;
  unsigned int n;
  long long unsigned int in;
  double pi;
  long unsigned int duracao;
  struct timeval start, end;

  scanf("%d %u",&size, &n);

  srand(time(NULL));
	
  gettimeofday(&start, NULL);
  in = monte_carlo_pi_parallel(n, size);
  gettimeofday(&end, NULL);
  duracao = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));

  pi = 4*in/((double)n);
  printf("%lf\n",pi);

  printf("%lu\n", duracao);

  return 0;
}
