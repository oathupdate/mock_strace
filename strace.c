#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <asm/unistd_64.h>
#include <stdint.h>
#include <linux/net.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include "strace.h"

extern int g_quit;

int UmovenPeekdata(const int pid, kernel_ulong_t addr,
    size_t len, void *laddr) {
  unsigned int nread = 0;
  unsigned int residue = addr & (sizeof(long) - 1);

  while (len) {
    addr &= -sizeof(long);
    errno = 0;
    union {
      long val;
      char x[sizeof(long)];
    } u = { .val = ptrace(PTRACE_PEEKDATA, pid, addr, 0) };

    switch (errno) {
      case 0:
        break;
      case ESRCH:
      case EINVAL: /* process is gone */
        return -1;
      case EFAULT:
      case EIO:
      case EPERM:  // address space is inaccessible
        return -1;
      default:
        return -1;
    }
    unsigned int m = (sizeof(long) - residue > len) ? len : sizeof(long) - residue;
    memcpy(laddr, &u.x[residue], m);
    residue = 0;
    addr += sizeof(long);
    laddr += m;
    nread += m;
    len -= m;
  }
  return 0;
}

void Run(Strace *s) {
  int status = 0, ret;
  char *buf = (char*)malloc(65535);
  pid_t  pid;
  size_t buf_len;
  kernel_ulong_t addr;

  ret = ptrace(PTRACE_ATTACH, s->pid, NULL, NULL);
  if (ret < 0) {
    printf("ERROR: PTRACE_ATTACH failed\n");
    return;
  }

  ptrace(PTRACE_SETOPTIONS, s->pid, NULL, PTRACE_O_TRACESYSGOOD);

  int insyscall = 0;
  while (!s->quit) {
    memset(&s->regs, 0, sizeof(struct user_regs_struct));
    pid = waitpid(s->pid, &status, __WALL);
    if (pid < 0) {
      break;
    }
    if (pid == 0) {
      ptrace(PTRACE_SYSCALL, s->pid, NULL, NULL);
      continue;
    }
    
    /* differnet kernals has diffrent rax */
    int64_t orig_rax = ptrace(PTRACE_PEEKUSER, s->pid, 8*ORIG_RAX, NULL);
    if (orig_rax != __NR_read && orig_rax != __NR_write) {
      goto next_syscall;
    }

    ret = ptrace(PTRACE_GETREGS, s->pid, NULL, &s->regs);
    if (ret < 0) {
      goto next_syscall;
    }
    if (insyscall == 0) {
      insyscall = 1;  /* syscall exit */
      goto next_syscall;
    }
    insyscall = 0;
    addr = s->regs.rsi;
    /*
     * orig_rax: message type
     * rdx: message len
     */
    switch (s->regs.orig_rax) {
      case __NR_read:
      case __NR_write:
        ret = UmovenPeekdata(s->pid, addr, s->regs.rdx, buf);
        buf_len = s->regs.rdx;
        printf("%.*s\n", buf_len, buf);
        break;
      default:
        break;
    }
next_syscall:
    ptrace(PTRACE_SYSCALL, s->pid, NULL, NULL);
  }
  free(buf);
  ptrace(PTRACE_DETACH, s->pid, NULL, NULL);
  printf("INFO: strace exit\n\n");
}
