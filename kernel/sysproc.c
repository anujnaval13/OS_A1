#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"

struct history_entry {
  int pid;
  char name[16];
  uint64 mem_usage;
};

#define HISTORY_SIZE 100
struct history_entry history[HISTORY_SIZE];
int history_count = 0;
int history_start = 0; // Points to the oldest entry

struct spinlock history_lock; // Ensure thread safety

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// Return how many clock tick interrupts have occurred since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_history(void)
{
  struct proc *p = myproc();
  uint64 user_buf;

  // Use argaddr to obtain the user buffer address.
  argaddr(0, &user_buf);

  if (history_count == 0)
    return 0; // No history yet

  // Copy history to user space.
  if (copyout(p->pagetable, user_buf, (char *)history,
              sizeof(struct history_entry) * history_count) < 0)
    return -1;

  return history_count;
}

void add_to_history(struct proc *p) {
  if (p == 0)
    return;

  // Skip adding "sh", "init", and "history" to history
  if (strncmp(p->name, "sh", 16) == 0 || 
      strncmp(p->name, "init", 16) == 0 || 
      strncmp(p->name, "history", 16) == 0)
    return;

  acquire(&history_lock); // Lock before modifying history

  int index = (history_start + history_count) % HISTORY_SIZE; // Circular index

  struct history_entry *h = &history[index];
  h->pid = p->pid;
  safestrcpy(h->name, p->name, sizeof(h->name));
  h->mem_usage = p->sz; // Memory occupied by process

  if (history_count < HISTORY_SIZE) {
    history_count++; // Increase count if not full
  } else {
    history_start = (history_start + 1) % HISTORY_SIZE; // Remove oldest entry
  }

  release(&history_lock); // Unlock after modification
}
