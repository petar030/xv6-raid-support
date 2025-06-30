#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "raid.h"
#include "fs.h"


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


uint64 sys_init_raid(void) {
    int n;
    argint(0, &n);
    return init_raid(n);
}

uint64 sys_read_raid(void) {
    int n;
    argint(0, &n);
    uint64 addr;
    argaddr(1, &addr);
    uchar buffer[BSIZE];

    if (read_raid(n, buffer) < 0)
        return -1;

    if (copyout(myproc()->pagetable, addr, (char *)buffer, BSIZE) < 0)
        return -1;

    return 0;
}

uint64 sys_write_raid(void) {
    int n;
    argint(0, &n);
    uint64 addr;
    argaddr(1, &addr);
    uchar buffer[BSIZE];

    if (copyin(myproc()->pagetable, (char *)buffer, addr, BSIZE) < 0)
        return -1;

    return write_raid(n, buffer);
}

uint64 sys_disk_fail_raid(void) {
    int n;
    argint(0, &n);
    return disk_fail_raid(n);
}

uint64 sys_disk_repaired_raid(void) {
    int n;
    argint(0, &n);
    return disk_repaired_raid(n);
}

uint64 sys_info_raid(void) {
    uint64 blkn, blks, diskn;
    argaddr(0, &blkn);
    argaddr(1, &blks);
    argaddr(2, &diskn);

    uint blkn_buffer, blks_buffer, diskn_buffer;
    if(info_raid(&blkn_buffer, &blks_buffer, &diskn_buffer) < 0)
        return -1;
    
    int len = sizeof(uint);

    if (copyout(myproc()->pagetable, blkn, (char *)&blkn_buffer, len) < 0)
        return -1;
    if (copyout(myproc()->pagetable, blks, (char *)&blks_buffer, len) < 0)
        return -1;
    if (copyout(myproc()->pagetable, diskn, (char *)&diskn_buffer, len) < 0)
        return -1;

    return 0;
}

uint64 sys_destroy_raid(void) {
    return destroy_raid();
}

