
#include <stdio.h>

long long unsigned int producer_consumer_serial(int *buffer, int size, int *vec, int n) {
  int i, j;
  long long unsigned int sum = 0;

  for(i = 0; i < n; i++) {
    if(i % 2 == 0) { // PRODUCER
      for(j = 0; j < size; j++) {
	buffer[j] = vec[i] + j * vec[i + 1];
      }
    }
    else { // CONSUMER
      for(j = 0; j < size; j++) {
	sum += buffer[j];
      }
    }
  }
  return sum;
}
