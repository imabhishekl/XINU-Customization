#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) 
{
  int i;
  STATWORD        ps;

  /* Check if a valid request */
  if(bs_id < 0 || bs_id > TOTAL_BS || npages < 1 || npages > MAX_PAGE_ALL_BS || bsm_tab[bs_id].is_private == YES)
  {
     //kprintf("Prov %d",bsm_tab[bs_id].is_private);
     kprintf("\nInvalid Request to get_bs");
     return SYSERR;
  }

  disable(ps);

  /* requests a new mapping of npages with ID map_id */
//  for(i = 0;i < TOTAL_BS;i++)
  //{
     if(bsm_tab[bs_id].bs_status == BSM_UNMAPPED)
     {
        //kprintf("\nRequested BS<%d> is UNMAPPED",bs_id);
        bsm_tab[bs_id].bs_status = BSM_MAPPED;
        bsm_tab[bs_id].bs_pid = currpid;
        bsm_tab[bs_id].bs_vpno = 0;
        bsm_tab[bs_id].bs_npages = npages;
        bsm_tab[bs_id].is_private = NO;
        bsm_tab[bs_id].next = NULL;
        proctab[currpid].bs_list[bs_id] = bsm_tab[bs_id];
     }
     else if(bsm_tab[bs_id].bs_status == BSM_MAPPED) /* Already Mapped BS so we need to add this also to the list */
     {
        //kprintf("\nRequested Bs is MAPPED");
        bs_map_t *proc_bs = &(proctab[currpid].bs_list[bs_id]);
        bs_map_t *temp = &bsm_tab[bs_id];

        npages = temp->bs_npages;

        while(temp->next != NULL)
	   temp = temp->next;

        //kprintf("\n<%d><%d>",npages,temp->bs_npages);
        npages = temp->bs_npages;
        proc_bs->bs_status = BSM_MAPPED;
        proc_bs->bs_pid = currpid;
        proc_bs->bs_vpno = 0;
        proc_bs->bs_npages = npages;
        temp->next = proc_bs;
     }
  //}
  //kprintf("Return %d from get_bs",npages);
  restore(ps);
  return npages;
}
