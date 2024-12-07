#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

/*struct event *event_list = 0;
static int event_count = 0;

void add_event(int pid, char *name, char *syscall, int ret){
  struct event *new_event = (struct event *)kalloc();
  if (!new_event){
    cprintf("Unable to allocate memory for new event\n");
    return;
  }

  new_event->pid = pid;
  strncpy(new_event->name, name, sizeof(new_event->name) -1);
  strncpy(new_event->syscall, syscall, sizeof(new_event->syscall) -1);
  new_event->ret = ret;
  new_event->next = event_list;

  event_list = new_event;
  event_count++;

  //remove old events if size exceeds N
  if (event_count > N){
    struct event *current = event_list;
    while(current->next->next){
      current = current->next;
    }
    kfree((char*)current->next);
    current->next=0;
    event_count--;
  }
}*/

#define N 35 // Maximum number of events to store

void add_event(struct proc *curproc, const char *syscall_name, int return_value) {
    struct event *new_event = (struct event *)kalloc();
    if (!new_event) {
        cprintf("Error: Unable to allocate memory for new event.\n");
        return;
    }

    new_event->pid = curproc->pid;
    strncpy(new_event->name, curproc->name, sizeof(new_event->name) - 1);
    new_event->name[sizeof(new_event->name) - 1] = '\0'; // Null-terminate
    strncpy(new_event->syscall, syscall_name, sizeof(new_event->syscall) - 1);
    new_event->syscall[sizeof(new_event->syscall) - 1] = '\0'; // Null-terminate
    new_event->ret = return_value;
    new_event->next = 0;

    // Add to the tail of the list
    if (curproc->event_tail) {
        curproc->event_tail->next = new_event;
    } else {
        curproc->event_head = new_event; // If list is empty, set head
    }
    curproc->event_tail = new_event;
    curproc->event_count++;

    // Remove the oldest event if the list exceeds N
    if (curproc->event_count > N) {
        struct event *old_event = curproc->event_head;
        curproc->event_head = old_event->next;
        if (!curproc->event_head) {
            curproc->event_tail = 0; // List is now empty
        }
        kfree((char *)old_event);
        curproc->event_count--;
    }
}
// Fetch the int at addr from the current process.
int
fetchint(uint addr, int *ip)
{
  struct proc *curproc = myproc();

  if(addr >= curproc->sz || addr+4 > curproc->sz)
    return -1;
  *ip = *(int*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(uint addr, char **pp)
{
  char *s, *ep;
  struct proc *curproc = myproc();

  if(addr >= curproc->sz)
    return -1;
  *pp = (char*)addr;
  ep = (char*)curproc->sz;
  for(s = *pp; s < ep; s++){
    if(*s == 0)
      return s - *pp;
  }
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  return fetchint((myproc()->tf->esp) + 4 + 4*n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  int i;
  struct proc *curproc = myproc();
 
  if(argint(n, &i) < 0)
    return -1;
  if(size < 0 || (uint)i >= curproc->sz || (uint)i+size > curproc->sz)
    return -1;
  *pp = (char*)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
  int addr;
  if(argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_trace(void);
extern int sys_dumptrace(void);
extern int sys_handleflags(void);
extern int sys_writebuffer(void);

static int (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
[SYS_trace]   sys_trace,
[SYS_dumptrace] sys_dumptrace,
[SYS_handleflags] sys_handleflags,
[SYS_writebuffer] sys_writebuffer,
};
static char *syscall_names[] = {
    "fork", "exit", "wait", "pipe", "read", "kill", "exec", "fstat", "chdir",
    "dup", "getpid", "sbrk", "sleep", "uptime", "open", "write", "mknod",
    "unlink", "link", "mkdir", "close", "trace", "dumptrace", "handleflags"
};
char* getname(int num) {
  switch (num) {
    case SYS_fork: return "fork";
    case SYS_exit: return "exit";
    case SYS_wait: return "wait";
    case SYS_pipe: return "pipe";
    case SYS_read: return "read";
    case SYS_kill: return "kill";
    case SYS_exec: return "exec";
    case SYS_fstat: return "fstat";
    case SYS_chdir: return "chdir";
    case SYS_dup: return "dup";
    case SYS_getpid: return "getpid";
    case SYS_sbrk: return "sbrk";
    case SYS_sleep: return "sleep";
    case SYS_uptime: return "uptime";
    case SYS_open: return "open";
    case SYS_write: return "write";
    case SYS_mknod: return "mknod";
    case SYS_unlink: return "unlink";
    case SYS_link: return "link";
    case SYS_mkdir: return "mkdir";
    case SYS_close: return "close";
    case SYS_trace: return "trace";
    case SYS_dumptrace: return "dumptrace";
    case SYS_handleflags: return "handleflags";
  }
  return "";
}

void
syscall(void)
{
  int num;
  int ret;
  struct proc *curproc = myproc();

  num = curproc->tf->eax;

  if (num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    // Execute the system call and save the return value
    ret = syscalls[num]();
    curproc->tf->eax = ret;

    // Check if tracing is enabled and exclude SYS_trace and SYS_dumptrace
    if (curproc->trace_on && num != SYS_trace && num != SYS_dumptrace) {
      int should_trace = 1; // Assume we trace by default

      // Apply flag-based filtering
      if (curproc->flags.e && strncmp(syscall_names[num], curproc->flags.syscall, sizeof(curproc->flags.syscall)) != 0) {
        should_trace = 0; // Skip if syscall doesn't match -e filter
      }

      if (curproc->flags.s && ret != -1) {
        should_trace = 0; // Skip if the system call succeeded and -s flag is set
      }

      if (curproc->flags.f && ret != -1) {
        should_trace = 0; // Skip if the system call succeeded and -f flag is set
      }

      // If we decided to trace, add the event and log the trace
      if (should_trace) {
        add_event(curproc, syscall_names[num], curproc->tf->eax);
        cprintf("TRACE: pid = %d | process name = %s | syscall = %s | return = %d\n",
                curproc->pid, curproc->name, syscall_names[num], ret);
      }
    }
  } else {
    // Handle unknown system call
    cprintf("%d %s: unknown sys call %d\n", curproc->pid, curproc->name, num);
    curproc->tf->eax = -1;
  }
}



/*
void
syscall(void)
{
  int num;
  struct proc *curproc = myproc();

  num = curproc->tf->eax;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    curproc->tf->eax = syscalls[num]();
  } else {
    cprintf("%d %s: unknown sys call %d\n",
            curproc->pid, curproc->name, num);
    curproc->tf->eax = -1;
  }
}*/
