/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
        int pid;
        int bs_id;
        struct pentry *proc;
        STATWORD        ps;
	kprintf("Calling the process via vcreate!\n");
        disable(ps);
        /* Searching for private BS for the process */
        if(get_bsm(&bs_id) == SYSERR)
        {
           //kprintf("\nFailed in vcreate");
           return SYSERR;
        }
        pid = create(procaddr,ssize,hsize,priority);
       
        bsm_tab[bs_id].bs_status = BSM_MAPPED;
        bsm_tab[bs_id].bs_pid = currpid;
        bsm_tab[bs_id].bs_vpno = 0;
        bsm_tab[bs_id].bs_npages = hsize;
        bsm_tab[bs_id].is_private = YES;
  
        /* vcreate assign the memory for vmemlist which is its heap */
        proc = &proctab[pid];
       
        proc->vmemlist->mnext = (struct mblock *) (BACKING_STORE_BASE + (bs_id*NBPG*128));
//        proc->vmemlist->mnext = (struct mblock *)NULL;
        proc->vmemlist->mnext->mlen = (hsize*NBPG);
        proc->store = bs_id;
        proc->vhpnpages = hsize;
        //proc->vhpno = 4096; /* Starting vitual address space of the process */
        restore(ps);
	return pid;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */

