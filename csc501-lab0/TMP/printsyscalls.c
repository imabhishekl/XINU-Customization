#include <stdio.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>

void syscallsummary_start()
{
   int i,j;
   sys_trace_flag = ON;
   kprintf("\nin start");
   for(i = 0;i < NPROC;i++)
   {
      syscallcount[i].syscall_used_ind = OFF;
      for(j = 0;j < SYSCALL_REQ;j++)
      {
         syscallcount[i].sys_call_count[j] = 0;
      }
   }
   sys_trace_flag = ON;
}

void syscallsummary_stop()
{
   sys_trace_flag = OFF;
}

void printsyscalls()
{
   int i;
   //syscallsummary_start();
   kprintf("\n|-------------------------------------------------- -------------------|");
   kprintf("\n PROCESS          | GETPID | GETTIME | KILL | SIGNAL | SLEEP | WAIT    ");
   kprintf("\n|----------------------------------------------------- ----------------|");
   for(i = 0;i < NPROC;i++)
   {
      if(syscallcount[i].syscall_used_ind == OFF)
         continue;

      kprintf("\n %s           |  %d     |  %d     | %d     |  %d     |  %d     |  %d     |",proctab[i].pname
                                                                       ,syscallcount[i].sys_call_count[0],syscallcount[i].sys_call_count[1]
                                                                       ,syscallcount[i].sys_call_count[2],syscallcount[i].sys_call_count[3]
                                                                       ,syscallcount[i].sys_call_count[4],syscallcount[i].sys_call_count[5]);
   }
}

/*int getProcIndex(char *proc_name)
{
   if(strncmp(proc_name,GETPID,PROC_NM_SZ))
   {
      return 0;
   }
   else if(strncmp(proc_name,GETTIME,PROC_NM_SZ))
   {
      return 1;
   }
   else if(strncmp(proc_name,KILL,PROC_NM_SZ))
   {
      return 2;
   }
   else if(strncmp(proc_name,SIGNAL,PROC_NM_SZ))
   {
      return 3;
   }
   else if(strncmp(proc_name,SLEEP,PROC_NM_SZ))
   {
      return 4;
   }
   else if(strncmp(proc_name,WAIT,PROC_NM_SZ))
   {
      return 5;
   }
   else
   {
      return 0;
   }
}*/
