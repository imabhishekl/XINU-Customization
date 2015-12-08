/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
  /* sanity check ! */
  STATWORD        ps;
  if ((virtpage < 4096) || ( source < 0 ) || ( source > TOTAL_BS) ||(npages < 1) || ( npages > 128) || bsm_tab[source].is_private == YES)
  {
	kprintf("xmmap call error: parameter error! \n");
	return SYSERR;
  }

  disable(ps);
  return bsm_map(currpid, virtpage, source, npages);
  restore(ps);
}

/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage )
{
  bs_map_t *temp_bs,*prev_bs;
  int i;
  STATWORD        ps;
  //kprintf("\nCalled XMUNMAP for <%d>",virtpage);
  /* sanity check ! */
  if ( (virtpage < 4096) ){ 
	kprintf("xmummap call error: virtpage (%d) invalid! \n", virtpage);
	return SYSERR;
  }
  disable(ps);
  write_back_data(currpid);

  for(i = 0; i < TOTAL_BS;i++)
  {
     if(bsm_tab[i].bs_status == BSM_UNMAPPED)
        continue;
     temp_bs = &bsm_tab[i];
     while(temp_bs != NULL)
     {
        if(temp_bs->bs_vpno == virtpage)
        {
           /* Write the BS if mapped to the main memory */
           
           if(prev_bs == NULL && temp_bs->next == NULL)
           {
              //write_back_data(currpid,i);
              /* This was the only mapping in the list and hence make the BSM available to use for other process */
              //kprintf("\nOnly Mapping");
              free_frm(i,temp_bs->bs_pid);
              free_bsm(i);
              //temp_bs->bs_status = BSM_UNMAPPED;
           }
           else
           {
              //write_back_data(currpid,i);
              //kprintf("\nRemoiving already mapped ");
              free_frm(i,temp_bs->bs_pid);
              temp_bs->bs_vpno = -1;
              //prev_bs->next = temp_bs->next; /* remove the node from the list as this is no more a valid mapping */
              restore(ps);
              return OK;
           }
        }
        prev_bs = temp_bs;
        temp_bs = temp_bs->next;
     }
  }
  restore(ps);
  return SYSERR;
}
