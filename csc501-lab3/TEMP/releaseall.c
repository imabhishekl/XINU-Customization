#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lock.h>

int releaseall (numlocks,arg_addr)
    int numlocks; 
    int arg_addr;
{
   STATWORD ps;
   disable(ps);
   
   unsigned long *lock_index = (unsigned long *)(&arg_addr); /* last argument      */
   int i;
   int lk_index;
   int lk_index_map = -1;
   order_list_t *list;
 
   for(i = 0;i < numlocks;i++)
   {
      lk_index_map = *lock_index++;
      //kprintf("\nRelease Argo   : %d",lk_index_map);
      if(isbadlock(lk_index_map) || is_proc_hold(lock_map[lk_index_map],currpid) == NO)
      {
         //kprintf("\nDOes not hold it!!");
         return SYSERR;
      } 

      lk_index = lock_map[lk_index_map];
      if(lock_tab[lk_index].lock_type == READ)
      {
         lock_tab[lk_index].read_count--;
         if(lock_tab[lk_index].read_count > 0)
            continue;
      }

      /* Check if something is present in the queue */
      if(is_present(lk_index) == EMPTY)
      {
         //kprintf("\nNothing in wakeup queue");
         restore(ps);
         lock_init(lk_index);
         continue;
      }
      else
      {
         //if((temp_pri = get_first_write(lk_index)) == EMPTY)
         list = get_first(lk_index);
         if(list->lock_type == READ)
         {
           /* Since the wining is reader process wake up all reader process untill writer comes */
           //kprintf("\nCurrent wait contains READ <%d>",list == NULL);
           //list = remove_first(lk_index);

           while(list != NULL)
           {
              list = get_first(lk_index);
              if(list->lock_type == READ)
              {
                 //kprintf("\nWaking a Read Process : <%d>",currpid);
                 list = remove_first(lk_index);
                 ready(list->pid,RESCHNO);
              }
              else
              {
                 /* Found the write leave it*/
                 break;
              }
           }
           //resched();
         }
         else if(list->lock_type == WRITE)
         {
            /* Since the writer is won wake it up */
            //kprintf("\nReleasing Write lock");
            lock_tab[lk_index].lock_type = WRITE;
            lock_tab[lk_index].curr_proc = list->pid;
            lock_tab[lk_index].curr_pri = list->pri;
            remove_first(lk_index);
            ready(list->pid,RESCHNO);
            //resched();
         }
         else
         {
            //kprintf("\nLock Type not set ");
         }
      }
   }
   resched();
   restore(ps);
   return OK;
}
