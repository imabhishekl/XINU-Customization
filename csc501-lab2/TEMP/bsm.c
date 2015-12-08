/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */

bs_map_t bsm_tab[TOTAL_BS];

SYSCALL init_bsm()
{
   int i;
   //kprintf("\n Initializing BSM");
   for(i = 0;i < TOTAL_BS;i++)
   {
      bsm_tab[i].bs_status = BSM_UNMAPPED;
      bsm_tab[i].bs_pid = -1;
      bsm_tab[i].bs_vpno = -1;
      bsm_tab[i].bs_npages = -1;
      bsm_tab[i].is_private = NO;
   }
   return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
   int i;
   /* Get the Free List from Backing Store */
   /* Searching for private BS for the process */
   for(i = 0;i < TOTAL_BS;i++)
   {
      if(bsm_tab[i].bs_status == BSM_UNMAPPED)
      {
         *avail = i;
         return OK;
      }
   }
   return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
   bsm_tab[i].bs_status = BSM_UNMAPPED;
   bsm_tab[i].bs_pid = -1;
   bsm_tab[i].bs_vpno = -1;
   bsm_tab[i].bs_npages = -1;
   bsm_tab[i].is_private = NO;
   bsm_tab[i].next = NULL;
   return OK;
}

SYSCALL free_bsm_ref(bs_map_t *map)
{
   map->bs_status = BSM_UNMAPPED;
   map->bs_pid = -1;
   map->bs_vpno = -1;
   map->bs_npages = -1;
   map->is_private = NO;
   map->next = NULL;
   return OK;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
   int  i;
   struct pentry *proc = &proctab[pid];
   for(i = 0;i < TOTAL_BS;i++)
   {
      /* Search for the virtual address requested */
      bs_map_t *proc_bs = &(proc->bs_list[i]);
      //kprintf("\n<%d><%d><%d><%d><%ld>",i,proc_bs->bs_pid,proc_bs->bs_vpno,proc_bs->bs_npages,vaddr);
      if(proc_bs->bs_status == BSM_MAPPED && ((proc_bs->bs_vpno + proc_bs->bs_npages)) >= vaddr && (proc_bs->bs_vpno) <= vaddr)
      {
         *store = i;
         *pageth = vaddr - proc_bs->bs_vpno;
         return OK;
      }
   }
}

/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
   /* Check is the mapped BS is used or not */
   bs_map_t *bs;
   //kprintf("\nIn BSM routine");
   if(bsm_tab[source].bs_status == BSM_MAPPED)
   {
      if(bsm_tab[source].bs_npages < npages)
      {
          //kprintf("\nRequeted more for mapping");
	  return SYSERR;
      }
      /* Add mapping for this process in the backing store */
      bs = &bsm_tab[source];
      while(bs != NULL)
      {
         //kprintf("\nMAP<%d><%d><%d>",bs->bs_pid,currpid,bs->bs_status);
         if(bs->bs_pid == currpid) /* The process which requested the mapping */
         {
            bs->bs_vpno = vpno;
            proctab[currpid].bs_list[source].bs_vpno = vpno;
            //kprintf("\nXMMAP<%d>,<%d>,<%d>,<%d>",currpid,source,proctab[currpid].bs_list[source].bs_npages,proctab[currpid].bs_list[source].bs_vpno);
            return OK;
         }
         bs = bs->next;
      }     
      return SYSERR; /* Get BS was not done for this process */
   }
   else
   {
      //kprintf("\nBacking store was not allocated");
      return SYSERR;
   }
}

/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
   
}
