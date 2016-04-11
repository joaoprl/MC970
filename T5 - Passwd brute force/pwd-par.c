#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

FILE *popen(const char *command, const char *type);

double rtclock()
{
  struct timezone Tzp;
  struct timeval Tp;
  int stat;
  stat = gettimeofday (&Tp, &Tzp);
  if (stat != 0) printf("Error return from gettimeofday: %d",stat);
  return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}

typedef struct{
  int *found;
  int start;
  int step;
  char *filename;
}Args;

void try(int *found, int start, int step, char* filename){
  FILE * fp;
  char finalcmd[300] = "unzip -P%d -t %s 2>&1";
  char ret[200];
  char cmd[400];
  int i;
  for(i = start; i < 500000; i += step){
    sprintf((char*)&cmd, finalcmd, i, filename);
    //printf("Comando a ser executado: %s \n", cmd);
    //printf("%d\n", i);

    fp = popen(cmd, "r");	
    while (!feof(fp)) {
      fgets((char*)&ret, 200, fp);
      if (strcasestr(ret, "ok") != NULL) {
	printf("Senha:%d\n", i);
	i = 500000;
	*found = 1;
      }
      else if(strcasestr(ret, "cannot find or open") != NULL){
	if(start == 0)
	  printf("File  not found\n");
	return;
      }	
    }
    pclose(fp);
    if(*found == 1)
      break;
  }

}

void* run_brute_force(void *args){
  Args* argsS = (Args *) args;
  try(argsS->found, argsS->start, argsS->step, argsS->filename);

  return NULL;
}

int main ()
{
  int nt;
  char *filename;
  filename = malloc(sizeof(char) * 100);
  double t_start, t_end;

  scanf("%d", &nt);
  scanf("%s", filename);
  pthread_t threads[nt];
  Args args[nt];
  int found = 0;
  int i;
  t_start = rtclock();
  for(i = 0; i < nt; i++){
    args[i] = (Args){&found, i + 10000, nt, filename};
    pthread_create(&(threads[i]), NULL, run_brute_force, (void *) &(args[i]));    
  }

  for(i = 0; i < nt; i++){
    pthread_join(threads[i], NULL);
  }
  
  t_end = rtclock();
 
  fprintf(stdout, "%0.6lf\n", t_end - t_start);
  free(filename);
}
