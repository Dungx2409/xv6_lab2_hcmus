#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
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
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgpte(void)
{
  uint64 va;
  struct proc *p;  

  p = myproc();
  argaddr(0, &va);
  pte_t *pte = pgpte(p->pagetable, va);
  if(pte != 0) {
      return (uint64) *pte;
  }
  return 0;
}
#endif

#ifdef LAB_PGTBL
int
sys_kpgtbl(void)
{
  struct proc *p;  

  p = myproc();
  vmprint(p->pagetable);
  return 0;
}
#endif
#ifdef LAB_PGTBL
#define MAX_PAGES_TO_CHECK 64  // Limit to 64 pages (fits in a uint64 bitmask)

uint64 sys_pgaccess(void) {
  uint64 start;
  int npages;
  uint64 abitsaddr;
  uint64 mask;
  uint64 abits;

  struct proc *p = myproc();
  argaddr(0, &start);
  argint(1, &npages);
  argaddr(2, &abitsaddr);

  if (npages <= 0 || npages > 64)
    return -1;

  mask = 1;
  abits = 0;
  for(int i = 0; i < npages; i++) {
    uint64 va = start + i * PGSIZE;
    pte_t *pte = walk(p->pagetable, va, 0);
    // Check if PTE exists and is valid before checking access bit
    if(pte && (*pte & PTE_V) && (*pte & PTE_A)) {
      abits |= mask;
      // Clear the access bit
      *pte &= ~PTE_A;
    }
    mask <<= 1;
  }

  if (copyout(p->pagetable, abitsaddr, (char *)&abits, sizeof(abits)) < 0)
    return -1;
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int pte_print_enabled = 0;

uint64
sys_signalPTE(void)
{
  int enable;
  
  argint(0, &enable);
  // Thiết lập flag
  if(enable != 0 && enable != 1)
    return -1;
  if(enable) {
    printf("PTE print enabled\n");
  } else {
    printf("PTE print disabled\n");
  }
  pte_print_enabled = enable;
  return 0;
}