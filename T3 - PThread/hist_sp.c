#include <stdio.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

/* funcao que calcula o minimo valor em um vetor */
double min_val(double * vet,int nval) {
  int i;
  double min;

  min = FLT_MAX;

  for(i = 0; i < nval; i++) {
    if(vet[i] < min)
      min =  vet[i];
  }
  return min;
}

/* funcao que calcula o maximo valor em um vetor */
double max_val(double * vet, int nval) {
  int i;
  double max;

  max = FLT_MIN;

  for(i = 0; i < nval; i++) {
    if(vet[i] > max)
      max =  vet[i];
  }	
  return max;
}

/* conta quantos valores no vetor estao entre o minimo e o maximo passados como parametros */
int * count(double min, double max, int * vet, int nbins, double h, double * val, int nval) {
  int i, j, count;
  double min_t, max_t;

  for(j = 0; j < nbins; j++) {
    count = 0;
    min_t = min + j * h;
    max_t = min + (j + 1) * h;
    for(i=0;i<nval;i++) {
      if(val[i] <= max_t && val[i] > min_t) {
	count++;
      }
    }

    vet[j] = count;
  }

  return vet;
}

int * count2(double min, double max, int * vet, int nbins, double h, double * val, int nval) {
  int i, j, count;
  double min_t, max_t;

  for(j = 0; j < nbins; j++) {
    count = 0;
    min_t = min + j * h;
    max_t = min + (j + 1) * h;
    for(i=0;i<nval;i++) {
      if(val[i] <= max_t && val[i] > min_t) {
	count++;
      }
    }
    __sync_fetch_and_add(&(vet[j]), count);
  }
  return vet;
}

typedef struct {
  double min; double max; int * vet; int nbins; double h; double * val; int nval;
}Args;
  

void *test(void *args)
{
  Args *argsS = (Args *) args;
  count2(argsS->min, argsS->max, argsS->vet, argsS->nbins, argsS->h, argsS-> val, argsS->nval);
  return NULL;
}

/* conta quantos valores no vetor estao entre o minimo e o maximo passados como parametros */
int * countP(double min, double max, int * vet, int nbins, double h, double * val, int nval, int nt) {
  nt = 8;
  pthread_t threads[nt];
  Args args[nt];
  int i;
  for(i = 0; i < nt; i++){
    args[i] = (Args){min, max, vet, nbins, h, &(val[nval * i / nt]), nval/nt};
    
    if(pthread_create(&(threads[i]), NULL, test, (void *)&(args[i]))){
      printf("Error creating thread \n");
      exit(1);
    }
  }
  for(i = 0; i < nt; i++)
    pthread_join(threads[i], NULL);
  return vet;
}

int main(int argc, char * argv[]) {
  double h, *val, max, min;
  int n, nval, i, *vet, *vetP, size;
  long unsigned int duracao, duracaoP;
  struct timeval start, end;

  scanf("%d",&size);

  /* entrada do numero de dados */
  scanf("%d",&nval);
  /* numero de barras do histograma a serem calculadas */
  scanf("%d",&n);

  /* vetor com os dados */
  val = (double *)malloc(nval*sizeof(double));
  vet = (int *)malloc(n*sizeof(int));
  vetP = (int *)malloc(n*sizeof(int));

  /* entrada dos dados */
  for(i=0;i<nval;i++) {
    scanf("%lf",&val[i]);
  }

  /* calcula o minimo e o maximo valores inteiros */
  min = floor(min_val(val,nval));
  max = ceil(max_val(val,nval));

  /* calcula o tamanho de cada barra */
  h = (max - min)/n;


  /* chama a funcao */
  gettimeofday(&start, NULL);
  vet = count(min, max, vet, n, h, val, nval);
  gettimeofday(&end, NULL);
  duracao = ((end.tv_sec * 1000000 + end.tv_usec) -	\
	     (start.tv_sec * 1000000 + start.tv_usec));
  printf("Serial: %lu\n", duracao);
  
  gettimeofday(&start, NULL);
  vetP = countP(min, max, vetP, n, h, val, nval, size);
  gettimeofday(&end, NULL);
  duracaoP = ((end.tv_sec * 1000000 + end.tv_usec) -	\
	     (start.tv_sec * 1000000 + start.tv_usec));
  printf("Parallel: %lu\n", duracao);

  int correct = 0;
  for(i=1;i<=n;i++) {
    if(vet[i] != vetP[i]){
      correct = 1;
      break;
    }
  }
  if(correct == 1)
    printf("False\n");
  printf("True\n");

  printf("Speed up: %f\n", (float)duracao / duracaoP);

  free(vetP);
  free(vet);
  free(val);

  return 0;
}
