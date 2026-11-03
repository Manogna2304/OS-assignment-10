#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

// Assignment 10: Virtual memory page table access
extern pte_t *walkpgdir(pde_t *pgdir, const void *va, int alloc);

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Assignment 10: Q1
int
sys_memstats(void)
{
  memstats();
  return 0;
}

// Assignment 10: Q2
int
sys_vtop(void)
{
  int va;
  pte_t *pte;

  if(argint(0, &va) < 0)
    return -1;

  pte = walkpgdir(myproc()->pgdir, (char*)va, 0);

  if(pte == 0 || !(*pte & PTE_P)) {
    cprintf("Invalid or unmapped virtual address: %x\n", va);
    return -1;
  }

  uint pa = PTE_ADDR(*pte) | (va & (PGSIZE - 1));

  cprintf("Virtual Address  : %x\n", va);
  cprintf("Physical Address : %x\n", pa);

  return pa;
}

// Assignment 10: Q3 - Display page information
int
sys_pageinfo(void)
{
  int va;
  pte_t *pte;

  if(argint(0, &va) < 0)
    return -1;

  pte = walkpgdir(myproc()->pgdir, (char*)va, 0);

  if(pte == 0 || !(*pte & PTE_P)) {
    cprintf("Page is not mapped: %x\n", va);
    return -1;
  }

  cprintf("\n--- PAGE INFORMATION ---\n");
  cprintf("Virtual Address : %x\n", va);
  cprintf("Physical Frame  : %x\n", PTE_ADDR(*pte) >> 12);
  cprintf("Present (P)     : %d\n", !!(*pte & PTE_P));
  cprintf("Writable (W)    : %d\n", !!(*pte & PTE_W));
  cprintf("User (U)        : %d\n", !!(*pte & PTE_U));

  return 0;
}


// Assignment 10: Q3 - Set page to read-only
int
sys_setreadonly(void)
{
  int va;
  pte_t *pte;

  if(argint(0, &va) < 0)
    return -1;

  pte = walkpgdir(myproc()->pgdir, (char*)va, 0);

  if(pte == 0 || !(*pte & PTE_P)) {
    cprintf("Cannot protect unmapped page: %x\n", va);
    return -1;
  }

  // Remove write permission.
  *pte &= ~PTE_W;

  // Flush the TLB to apply the new permissions.
  lcr3(V2P(myproc()->pgdir));

  cprintf("Page at virtual address %x is now READ-ONLY\n", va);

  return 0;
}
