#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

long long unsigned int producer_consumer_parallel(int **buffer, int size, int *vec, int n, int nt) {
  int i, j;
  long long unsigned int sum = 0;
  for(i = 0; i < n; i++) // set flags to zero
    buffer[i][0] = 0;
  
#pragma omp parallel for num_threads(nt) default(none) shared(buffer, vec, n, size) private(i, j) reduction(+:sum)
  for(i = 0; i < n; i++) {
    if(i % 2 == 0) { // Producer
      for(j = 0; j < size; j++) {
	buffer[i + 1][j + 1] = vec[i] + j * vec[i + 1];
      }
      buffer[i + 1][0] = 1; // Product is ready
      
    }
    else { // Consumer
      while(buffer[i][0] == 0){ // Is product ready?
#pragma omp flush
      }
      
      for(j = 0; j < size; j++)
	sum += buffer[i][j + 1]; // save result
    }
  }
  return sum; // Return result
}

int main() {
  double start, end;
  int i, n, size, nt;
  int **buff;
  int *vec;
  long long unsigned int result;

  scanf("%d %d %d", &nt, &n, &size);
	
  buff = (int **) malloc(n * sizeof(int*)); // one buffer for each product
  for(i = 0; i < n; i ++)
    buff[i] = (int *) malloc((size + 1) * sizeof(int));
  vec = (int *) malloc(n * sizeof(int));

  for(i = 0; i < n; i++)
    scanf("%d", &vec[i]);
  
  start = omp_get_wtime();
  result = producer_consumer_parallel(buff, size, vec, n, nt);
  end = omp_get_wtime();
  
  printf("%llu\n", result);
  printf("%lf\n", end - start);

  for(i = 0; i < n; i++)
    free(buff[i]);
  free(buff);
  free(vec);

  return 0;
}
