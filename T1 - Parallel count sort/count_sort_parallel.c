
#include <stdlib.h>
#include <omp.h>
#include <string.h>

double count_sort_parallel(double a[], int n, int nt) {
  int i, j, count; /* counters */
  double *temp; /* temporary array to store sorted values */
  double start, end; /* time variables */

  temp = (double *)malloc(n * sizeof(double));
	
  start = omp_get_wtime();

  /* threads should have their own count, i and j; but a, temp and n should be shared */
#pragma omp parallel for num_threads(nt) default(none) private(count, i, j) shared(a, temp, n)
  for (i = 0; i < n; i++) {
    count = 0;
    for (j = 0; j < n; j++){
      if (a[j] < a[i]) /* smaller values */
	count++;
      else if (a[j] == a[i] && j < i) /* equals but came before */
	count++;
    }
    temp[count] = a[i];
  }

  end = omp_get_wtime();

  /* copy to original array and free memory */
  memcpy(a, temp, n * sizeof(double));
  free(temp);

  /* return the time */
  return end - start;
}
