#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/pstat.h"

const int PROCESSCES_NUM = 3;

static uint
fibonacci(uint n) {
  if (n <= 1) {
    return n;
  }
  return fibonacci(n - 1) + fibonacci(n - 2);
}

static int
get_ticks(struct pstat* pstat, int pid)
{
  for (int i = 0; i < NPROC; i++) {
    if (pstat->inuse[i] == 1 && pstat->pid[i] == pid) {
      return pstat->ticks[i];
    }
  }

  return -1;
}

static void
report(int children_pids[PROCESSCES_NUM])
{
  struct pstat pstat;
  int ticks[PROCESSCES_NUM];
  int finished_child = 0;
  while(1) {
    sleep(100);

    if (getpinfo(&pstat) < 0) {
      break;
    }

    for (int i = 0; i< PROCESSCES_NUM; i++) {
      ticks[i] = get_ticks(&pstat, children_pids[i]);
      if (ticks[i] < 0) {
        finished_child = 1;
        break;
      }
    }

    if (finished_child == 1) {
      break;
    }

    for (int i = 0; i< PROCESSCES_NUM; i++) {
      printf("%d\t", ticks[i]);
    }
    printf("\n");
  }
}

int
main(int argc, char *argv[])
{
  settickets(1000);

  int children_pids[PROCESSCES_NUM];

  for (int i = 0; i < PROCESSCES_NUM; i++) {
    int pid = fork();
    if (pid < 0) {
      printf("자식 농사 실패\n");
      exit(1);
    } else if (pid == 0) {
      settickets((i + 1) * 10);
      fibonacci(50);
      exit(0);
    } else {
      children_pids[i] = pid;
    }
  }

  int pid = fork();
  if (pid < 0) {
    printf("자식 농사 실패\n");
    exit(1);
  } else if (pid == 0) {
    settickets(10);
    report(children_pids);
    exit(0);
  }

  for (int i = 0; i < PROCESSCES_NUM + 1; i++) {
    wait(0);
  }
  printf("-- terminated\n");
  return 0;
}
