#include<stdio.h>
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>

void linit()
{
   int i = 0;
   lock_tab_t *lptr;
   for (i=0 ; i<NLOCKS ; i++) {      /* initialize LOCKS */
      (lptr = &lock_tab[i])->lstate = LFREE;
      lptr->head = NULL;
      lptr->curr_status = FREE;
      lptr->curr_proc = -1;
      lptr->read_count = 0;
   }
}
