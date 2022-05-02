#include <stdio.h>
#include <memory.h>

#include "strace.h"


int main(int argc, char **argv) {
  int     pid;
  Strace  s;

  if (argc != 2) {
    printf("ERROR: check args number falied\n");
    return 1;
  }
  pid = atoi(argv[1]);
  if (pid == 0) {
    printf("ERROR: check args pid failed\n");
    return 1;
  }

  memset(&s, 0, sizeof(Strace));
  s.pid = pid;
  Run(&s);
  return 0;
}
