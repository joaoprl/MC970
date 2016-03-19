#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "prod_cons.h"

int main() {
  double startSerial, endSerial, startParallel, endParallel;
  int i, n, size, nt;
  int **buff;
  int *vec;
  long long unsigned int resultSerial, resultParallel;

  scanf("%d %d %d", &nt, &n, &size);
	
  buff = (int **) malloc(n * sizeof(int*));
  for(i = 0; i < n; i ++)
    buff[i] = (int *) malloc((size + 1) * sizeof(int));
  vec = (int *) malloc(n * sizeof(int));

  for(i = 0; i < n; i++)
    scanf("%d", &vec[i]);
	
  startSerial = omp_get_wtime();
  resultSerial = producer_consumer_serial(buff[0], size, vec, n);
  endSerial = omp_get_wtime();
  
  startParallel = omp_get_wtime();
  resultParallel = producer_consumer_parallel(buff, size, vec, n, nt);
  endParallel = omp_get_wtime();
  

  printf("Correct: %s\n", (resultParallel == resultSerial)? "true" : "false");
  printf("Serial time: %lf\n", endSerial - startSerial);
  printf("Parallel time: %lf\n", endParallel - startParallel);

  for(i = 0; i < n; i++)
    free(buff[i]);
  free(buff);
  free(vec);

  return 0;
}
