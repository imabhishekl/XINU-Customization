/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
SYSCALL getpid()
{
        if(sys_trace_flag == ON)
        {
           syscallcount[currpid].syscall_used_ind = USED;
           syscallcount[currpid].sys_call_count[GETPID]++;
        }

	return(currpid);
}
