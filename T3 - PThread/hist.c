/*

1    2   4   8   16
  E 0.8 0.5 0.2 0.03
  S 1.6 2.0 1.3 0.5
2 
  E 1.0 0.6 0.3 0.2
  S 1.9 2.3 2.5 3.3
3
  E 1.0 0.6 0.3 0.2
  S 1.9 2.3 2.7 3.4

Para o arquivo 1, a queda no speedup pode ser atribuída ao tamanho reduzido do vetor (apenas 1000).
Nos arquivos 2 e 3, houve aumento no speedup, confome esperado; entretanto, a queda na eficiência pode ser atribuída a limitação da máquina utilizada para os testes.

Para profiling foi utilizado gprof no arq3.in fornecido. O resultado foi confome segue:
Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  ms/call  ms/call  name    
100.29      0.47     0.47        2   235.68   235.68  count2
  0.00      0.47     0.00        1     0.00     0.00  countP
  0.00      0.47     0.00        1     0.00     0.00  max_val
  0.00      0.47     0.00        1     0.00     0.00  min_val

% time This is the percentage of the `total' time that was spent
in this function and its children. Note that due to
different viewpoints, functions excluded by options, etc,
these numbers will NOT add up to 100%.

O código é altamente paralelizável, visto que em torno de 100% do tempo de execução se deve a count2 (parte paralelizada do código).

Utilizando perf também com a execução do arquivo 3:

Performance counter stats for './T3':
        484.295120      task-clock (msec)         #    1.829 CPUs utilized          
                12      context-switches          #    0.025 K/sec                  
                 3      cpu-migrations            #    0.006 K/sec                  
               367      page-faults               #    0.758 K/sec                  
     1,614,199,971      cycles                    #    3.333 GHz                    
   <not supported>      stalled-cycles-frontend  
   <not supported>      stalled-cycles-backend   
                 0      instructions              #    0.00  insns per cycle        
       234,987,904      branches                  #  485.216 M/sec                  
        20,375,006      branch-misses             #    8.67% of all branches        

       0.264801259 seconds time elapsed

 */
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
  int n, nval, i, *vet, size;
  long unsigned int duracao;
  struct timeval start, end;

  scanf("%d",&size);

  /* entrada do numero de dados */
  scanf("%d",&nval);
  /* numero de barras do histograma a serem calculadas */
  scanf("%d",&n);

  /* vetor com os dados */
  val = (double *)malloc(nval*sizeof(double));
  vet = (int *)malloc(n*sizeof(int));

  for(i = 0; i < n; i++)
    vet[i] = 0;
  
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
  vet = countP(min, max, vet, n, h, val, nval, size);
  gettimeofday(&end, NULL);
  duracao = ((end.tv_sec * 1000000 + end.tv_usec) -	\
	     (start.tv_sec * 1000000 + start.tv_usec));

  printf("%.2lf", min);
  for(i = 1; i <= n; i++)
    printf(" %.2lf", min + h * i);
  printf("\n");

  printf("%d", vet[0]);
  for(i = 1; i < n; i++)
    printf(" %d", vet[i]);
  printf("\n");
  
  for(i = 1; i < n; i++)
    printf(" %d", vet[i]);
  printf("\n");
  
  printf("%lu\n", duracao);

  free(vet);
  free(val);

  return 0;
}
