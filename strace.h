#ifndef STRACE_H
#define STRACE_H

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/user.h>

#define kernel_ulong_t unsigned long int

struct user_regs_struct;

typedef struct {
  pid_t pid;
  int   quit;
  struct user_regs_struct  regs;
} Strace;

void Run(Strace *s);

#endif
