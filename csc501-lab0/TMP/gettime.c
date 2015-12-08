/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <proc.h>
#include <lab0.h>

extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
SYSCALL	gettime(long *timvar)
{
    /* long	now; */

	/* FIXME -- no getutim */

    if(sys_trace_flag == ON)
    {
       syscallcount[currpid].syscall_used_ind = ON;
       syscallcount[currpid].sys_call_count[GETTIME]++; 
    }

    return OK;
}
