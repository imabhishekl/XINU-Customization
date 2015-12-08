#ifndef _SYSCALLTRACE_H_
#define _SYSCALLTRACE_H_
#define  OFF    0
#define ON      1
#define USED    1
#define GETPID  1
#define GETTIME 2
#define KILL    3
#define SIGNAL  4
#define SLEEP   5
#define WAIT    6
#define PROC_NM_SZ 8
#define SYSCALL_REQ 6
static int sys_trace_flag = ON;

long zfunction(long);
int  printsegaddress();
void printtos();
void printprocstks(int);
void printsyscalls();
void syscallsummary_start();
void syscallsummary_stop();


typedef struct proc_sys_detail
{
   int sys_call_count[SYSCALL_REQ];
   int syscall_used_ind;
}procsysdt;

procsysdt syscallcount[NPROC];
#endif
