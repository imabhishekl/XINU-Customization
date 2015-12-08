#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lock.h>

int lock(int ldes, int type, int priority)
{
   STATWORD ps;
   lock_tab_t *lk;   
   order_list_t *temp_ol;
   int high_write_pres = 0;
   int ldes1 = -1;

   disable(ps);

//   kprintf("\n Lock <%d><%d>",ldes,lock_map[ldes]);
   
   if (isbadlock(ldes) || lock_tab[lock_map[ldes]].lstate==LFREE) 
   {
      //kprintf("\nBad Lock");
      restore(ps);
      return(SYSERR);
   }
   ldes1 = lock_map[ldes];

   /* Need to add logic so that to check whether lock was deleted for this process */

   putin_proc_list(ldes1,currpid);

   /* If the lock requested is free that is no one  is holding it then aquire it and proceed */
   lk = &lock_tab[ldes1];
   //kprintf("\nlk<%d><%d>",lk->lock_type,lk->curr_status);
   if(lk->curr_status == USED)
   {
      /*  Since the lock is not free put it to wait list if a writer lock is on hold */
      //kprintf("\n Second type <%d><%d><%d>",type,lk->lock_type,is_write_pres(ldes1));
      if(is_write_pres(ldes1) != EMPTY)
      {
         temp_ol = get_first_write(ldes1);
         high_write_pres = temp_ol->pri > priority;
         //kprintf("\nhigh_write_pres <%d>",high_write_pres);
      }
     
      if(type == READ && lk->lock_type == READ && !high_write_pres)
      {
         //kprintf("\nRead process lock");
         lk->read_count++;
         lk->curr_proc = currpid;
         lk->curr_pri = priority;
         lk->lock_type = type;
         restore(ps);
         return OK;
      }
      else
      {
         //kprintf("\nLocked process <%d>",currpid);

         if(priority_insert(ldes1,currpid,priority,type) != OK)
         {
            //kprintf("\nFailed to insert");
            restore(ps);
            return SYSERR;
         }
         else
         {
            //kprintf("\nSucess to insert");
            //print_list(ldes1);
            proctab[currpid].pwaitret = OK;
            proctab[currpid].pstate = PRWAIT;
            resched();
            restore(ps);
            return proctab[currpid].pwaitret;
         }
      }
   }
   else
   {
      /* Lock not being used so */
      //kprintf("\nFirst Type <%d>",type);
      lk->curr_status = USED;
      lk->curr_proc = currpid;
      lk->curr_pri = priority;
      lk->lock_type = type;
      if(type == READ)
         lk->read_count++;
   }
   restore(ps);
   return OK;
}
