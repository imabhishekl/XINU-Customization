#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lock.h>
#include <sleep.h>

int priority_insert(int lock_index,int pid,int pri,int type)
{
   /*if (isbadlock_no(lock_index)) 
   {
      kprintf("\nBad Lock ID");
      return(SYSERR);
   }*/
   order_list_t *list = lock_tab[lock_index].head;
   order_list_t *new = NULL;
   order_list_t *prev = NULL;

   //kprintf("\nIn priority insert <%d>", list == NULL);
   if(list == NULL)
   {
      new = (order_list_t *)getmem(sizeof(order_list_t));
      new->pid = pid;
      new->pri = pri;
      new->time = ctr1000;
      new->lock_type = type;
      //kprintf("\nFirst Entry in Q");
      new->next = list;
      list = new;
      lock_tab[lock_index].head = list;
      //print_list(lock_index);
      return OK;
   }

   while(list != NULL)
   {
      //kprintf("\nIn While loop<%d>",list->lock_type);
      if(pri >= list->pri)
      {
         //kprintf("\nNot First in Q");
         new = (order_list_t *)getmem(sizeof(order_list_t));
         new->pid = pid;
         new->pri = pri;
         new->lock_type = type;
         new->time = ctr1000;
         if((pri == list->pri && type == WRITE && (ctr1000 - new->time) < 1000) || pri > list->pri)
         {
            //kprintf("\nInserting");
            if(prev == NULL)
            {
                new->next = list;
                list = new;
                lock_tab[lock_index].head = list;
            }
            else
            {
               prev->next = new;
               new->next = list;
            }
         }
         else
         {
            new->next = list->next;
            list->next = new;
         }
         return OK;
      }
      //kprintf("\nDoing next");
      prev = list;
      list = list->next;
   }
   /* Insert at the last element */
   new = (order_list_t *)getmem(sizeof(order_list_t));
   new->pid = pid;
   new->pri = pri;
   new->lock_type = type;
   new->time = ctr1000;
   new->next = NULL;
   prev->next = new;
   //kprintf("\nAfter while loop");
   //print_list(lock_tab[lock_index].head);
   return OK;
}

void print_list(int lock_index)
{
   kprintf("\nprint_list\n");
   order_list_t *list = lock_tab[lock_index].head;
   while(list != NULL)
   {
      kprintf("<%d|%d>-",list->pid,list->lock_type);
      list = list->next;
   }
}

int is_write_pres(int lock_index)
{
   order_list_t *list = lock_tab[lock_index].head;
   while(list != NULL)
   {
      if(list->lock_type == WRITE)
      {
         //kprintf("\nPRESENT");
         return PRESENT; 
      }
      list = list->next;
   }
   //kprintf("\nEMPTY");
   return EMPTY;
}

order_list_t* get_first_write(int lock_index)
{
   order_list_t *list = lock_tab[lock_index].head;
   while(list != NULL)
   {
      if(list->lock_type == WRITE)
      {
         //kprintf("\nPRESENT");
         return list;
      }
   }
   //kprintf("\nEMPTY");
   return NULL;
}

int is_present(int lock_index)
{
   order_list_t *head;
   head = lock_tab[lock_index].head;

   if(head == NULL)
   {
      return EMPTY;
   }
   else
   {
      return PRESENT;
   }
}

order_list_t* remove_first(int lock_index)
{
   if(lock_tab[lock_index].head == NULL)
   {
      return NULL;
   }
   order_list_t *first = lock_tab[lock_index].head;
   lock_tab[lock_index].head = lock_tab[lock_index].head->next;
   return first;
}

order_list_t* get_first(int lock_index)
{
   return lock_tab[lock_index].head;
}

int getfirst_ll(int lock_index)
{
   if(lock_tab[lock_index].head == NULL)
      return EMPTY;
   return lock_tab[lock_index].head->pid;
}

void putin_proc_list(int ldes1,int pid)
{
   if(ldes1 < NLOCKS)
   {
      proctab[pid].lock_list[ldes1] = 1;
      proctab[pid].lock_counter++;
      //kprintf("\nSetting the Process <%d> with ldesc<%d><%d>",pid,ldes1,proctab[pid].lock_list[ldes1]);
   }
}
int is_proc_hold(int lk_index,int pid)
{
   //kprintf("\nCalled Process <%d> with ldesc<%d><%d>",pid,lk_index,proctab[pid].lock_list[lk_index]);
   if(proctab[pid].lock_list[lk_index] == 1)
   {
      proctab[pid].lock_list[lk_index] = 0;
      proctab[pid].lock_counter--;
      return YES;
   }
   else
   {
      return NO;
   }
}

void lock_init(int index)
{
   if(index < NLOCKS)
   {
      lock_tab[index].head = NULL;
      lock_tab[index].curr_status = FREE;
      lock_tab[index].curr_proc = -1;
      lock_tab[index].read_count = 0;
   }
}
