/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <sched.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
int epoch_count;
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;
        int epoch_end = NO;
        int i;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);

        /* Update this process for epoch */
        //kprintf("proc %d,%s was requested to kill",pid,proctab[pid].pname);
        
        if(proctab[pid].is_create_real == YES)
        {
                if(epoch_count > 100)
			epoch_count = epoch_count - 100; 
                else
			epoch_count = 0;
                proctab[pid].is_create_real = NO;
        }
        else
        {
		proctab[pid].is_curr_epoch = NO;
        	epoch_count = epoch_count - proctab[pid].remaining_tick;
                //kprintf("\nRemainging Tick<%d>",proctab[pid].remaining_tick);
        	proctab[pid].remaining_tick = 0;
                for(i=1;i<NPROC;i++)
                {
			if(proctab[i].is_curr_epoch == YES) {epoch_end = YES;break;}
                }
                if(epoch_end == YES) epoch_count = 0;
	}
        //kprintf("\nepoch_count<%d>",epoch_count);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}
