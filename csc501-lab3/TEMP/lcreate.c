/* screate.c - screate, newsem */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

LOCAL int newlock();

/*------------------------------------------------------------------------
 * screate  --  create and initialize a lock_tabore, returning its id
 *------------------------------------------------------------------------
 */
SYSCALL lcreate()
{
	STATWORD ps;    
	int	lock;
        int     index;

	disable(ps);
	if ( (lock=newlock())==SYSERR ) {
		restore(ps);
		return(SYSERR);
	}
	//lock_tab[lock].lkcnt = count;
	/* sqhead and sqtail were initialized at system startup */
        /* Set the lock selected for using */
        /* Map the lock */
        if(map_pos >= TOTAL_MAPS)
        {
           restore(ps);
           return(SYSERR);
        }
        lock_map[map_pos] = lock;
	restore(ps);
	return(map_pos++);
}

/*------------------------------------------------------------------------
 * newsem  --  allocate an unused lock_tabore and return its index
 *------------------------------------------------------------------------
 */
LOCAL int newlock()
{
	int	lock_index;
	int	i;

	for (i=0 ; i<NLOCKS ; i++) {
		lock_index=nextlock--;
		if (nextlock < 0)
			nextlock = NLOCKS-1;
		if (lock_tab[lock_index].lstate==LFREE) {
			lock_tab[lock_index].lstate = LUSED;
			return(lock_index);
		}
	}
	return(SYSERR);
}
