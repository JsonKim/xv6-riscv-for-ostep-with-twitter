#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  struct pstat pstat;
  
  if (getpinfo(&pstat) < 0) {
    printf("ps: getpinfo failed\n");
    exit(1);
  }

  printf("PID\tTICKETS\tTICKS\n");
  
  for (int i = 0; i < NPROC; i++) {
    if (pstat.inuse[i] == 1) {
      printf("%d\t%d\t%d\n", pstat.pid[i], pstat.tickets[i], pstat.ticks[i]);
    }
  }

  exit(0);
}