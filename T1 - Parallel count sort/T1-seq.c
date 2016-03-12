
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "count_sort.h"

int main() {
  int i, n, nt;
  double  *a, *b, t_p, t_s;

  /* number of threads */
  scanf("%d", &nt);
	
  /* array size */
  scanf("%d", &n);

  /* create and load array */
  a = (double *) malloc(n * sizeof(double));
  for(i = 0; i < n; i++)
    scanf("%lf", &a[i]);
  b = (double *) malloc(n * sizeof(double));
  memcpy(b, a, n * sizeof(double));
	
  /* parallel and serial count sort call */
  t_s = count_sort_serial(a, n);
  t_p = count_sort_parallel(b, n, nt);
	
  /* Print array */
  /*
  for(i = 0; i < n; i++)
    printf("%.2lf ",a[i]);
  printf("\n");
  */

  /* Print true if correct, false otherwise */
  for(i = 0; i < n; i++)
    if(a[i] != b[i])
      printf("false\n");
  if(i == n)
    printf("true\n");

  /* Print time */
  printf("Serial: %lf\n", t_s);
  printf("Parallel: %lf\n", t_p);

  return 0;
}
