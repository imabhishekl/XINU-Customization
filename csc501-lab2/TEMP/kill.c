/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
        int i;
        bs_map_t *temp;
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

        /* Handle BS Mapping */
        for(i = 0;i < TOTAL_BS;i++)
        {
           //kprintf("\nWriting from Kill <%d>",pid);
           write_back_data(pid);
	   temp = &bsm_tab[i];
           while(temp !=NULL)
	   {
              //kprintf("\nWriting from Kill <%d><%d>",pid,temp->bs_pid);
	      if(temp->bs_pid == pid)
	      {
                 //kprintf("\nkill for pid<%d>",pid);
                 xmunmap(temp->bs_vpno);
                 release_bs(i);
	      }
	      temp=temp->next;
	   }
	}			
        //kprintf("\nWriting from Kill <%d>",pid);
        //write_back_data(pid);
        free_frm_kill(pid);
        //free_frame(6);

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
                        /* Need to release the BS for this process */
	}
	restore(ps);
	return(OK);
}
