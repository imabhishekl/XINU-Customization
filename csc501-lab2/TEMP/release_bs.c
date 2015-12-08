#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) 
{
  bs_map_t *temp_bs,*prev_bs;
  int i;

  /* release the backing store with ID bs_id */

  //kprintf("\nRealease BS<%d>",bs_id);
  if(bsm_tab[bs_id].bs_status == BSM_UNMAPPED)
  {
     //kprintf("\nRelease BS<%d> is unmapped",bs_id);
     return OK;
  }
  temp_bs = &bsm_tab[bs_id];
  while(temp_bs != NULL)
  {
     if(temp_bs->bs_pid == currpid)
     {
        /* Write the BS if mapped to the main memory */

        if(prev_bs == NULL && temp_bs->next == NULL)
        {
           /* This was the only mapping in the list and hence make the BSM available to use for other process */
           //kprintf("\nOnly Mapping,Releasing it");
           free_bsm(bs_id);
           //temp_bs->bs_status = BSM_UNMAPPED;
           return OK;
        }
        else if(prev_bs == NULL && temp_bs->next != NULL)
        {
           /* First one getting unmapped */
           temp_bs = temp_bs->next;
           //kprintf("\nFirst BSM getting unmapped<%d><%d>",bsm_tab[bs_id].bs_npages,temp_bs->bs_npages);
           //bsm_tab[bs_id].bs_status = temp_bs->bs_status;
           /*bsm_tab[bs_id].bs_pid = temp_bs->next->bs_status;
           bsm_tab[bs_id].bs_vpno = temp_bs->next->bs_vpno;
           bsm_tab[bs_id].bs_npages = temp_bs->next->bs_npages;
           bsm_tab[bs_id].is_private = temp_bs->next->is_private;
           bsm_tab[bs_id].next = temp_bs->next->next;*/
           bsm_tab[bs_id] = *temp_bs;
           free_bsm_ref(temp_bs);
           return OK;
        }
        else
        {
           //kprintf("\nRemoving already mapped ");
           free_bsm_ref(temp_bs);
           prev_bs->next = temp_bs->next; /* remove the node from the list as this is no more a valid mapping */
           return OK;
        }
     }
     prev_bs = temp_bs;
     temp_bs = temp_bs->next;
  }
  return OK;
}
