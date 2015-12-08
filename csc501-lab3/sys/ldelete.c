/* ldelete.c - ldelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * ldelete  --  delete a lock by releasing its table entry
 *------------------------------------------------------------------------
 */
SYSCALL ldelete(int lock_id)
{
	STATWORD ps;    
	lock_tab_t *lk;
        order_list_t *ol;
        int lock_index = -1;
        //kprintf("\nDeleting Lock<%d>",lock_map[lock_id]);

	disable(ps);
	if (isbadlock(lock_id) || lock_tab[lock_map[lock_id]].lstate==LFREE) {
                kprintf("\nInvalid Lock for delete");
		restore(ps);
		return(SYSERR);
	}

        lock_index = lock_map[lock_id];
        lock_map[lock_id] = -1;
	lk = &lock_tab[lock_index];
	lk->lstate = LFREE;
        lk->curr_proc = -1;
        lk->read_count = 0;
        lk->curr_status = FREE;
        
	if (is_present(lock_index)) 
        {
	   while( (ol = get_first(lock_index)) != NULL)
           {
              remove_first(lock_index);
	      proctab[ol->pid].pwaitret = DELETED;
	      ready(ol->pid,RESCHNO);
           }
           resched();
	}
	restore(ps);
	return(OK);
}
