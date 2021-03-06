/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>


/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{
  //kprintf("\nIn Interupt Service Routine!");
  /* Read the Page Fault Address */
  int store, pageth;
  pd_t *page_directory_entry;
  pt_t *page_table,*temp;
  int frm_no;
  int k;
  
  //disable(ps);
  unsigned int pfa = read_cr2();  /* Read the page Fault address */
  unsigned int pdb = proctab[currpid].pdbr;//proctab[currpid].pdbr;  /* Read the PDBR of process caused fault */
  unsigned int page_fr_no = (pfa >> 12);
  //kprintf("\nPage Fault Address Generated is <%08x><%d>",pfa,pfa,page_fr_no);

  /* Get the PDE,PTE and Page Offset to replace the page */
  unsigned int pde = pfa >> 22;
  unsigned int pte = (pfa >> 12) & 0x000003FF;
  unsigned int pf_offset = pfa & 0x00000FFF;
  //kprintf("\nPDB <%08x>",pdb);
  page_directory_entry = (pd_t *)(pdb + pde*sizeof(int)); /* Base + offset */

  if(page_directory_entry->pd_pres == 0)
  {
     frm_no = get_frm(page_fr_no,FR_TBL); /* Get a frame for page table  for pfa address without 4096*/

     if(frm_no == SYSERR)
     {
        //restore(ps);
	return SYSERR;
     }
  
     //bzero((frm_no + FRAME0) << 12,NBPG);

     /* We got a frame for page directory now set its value */
     page_directory_entry->pd_pres = 1;
     page_directory_entry->pd_write = 1;
     page_directory_entry->pd_user = 0;
     page_directory_entry->pd_pwt = 0;
     page_directory_entry->pd_pcd = 0;
     page_directory_entry->pd_acc = 0;
     page_directory_entry->pd_mbz = 0;
     page_directory_entry->pd_fmb = 0;
     page_directory_entry->pd_global = 0;
     page_directory_entry->pd_avail = 0;
     page_directory_entry->pd_base = (FRAME0 + frm_no);

     temp = (pt_t *)(page_directory_entry->pd_base << 12);
     for(k = 0;k < 1024;k++)
     {
        temp->pt_pres = 0;
        temp->pt_write = 1;
        temp->pt_base = 0;
        temp++;
     }

     //write_cr3(pdb*NBPG);
     //restore(ps);
     return OK;
  }
  else
  {
     //kprintf("\nPD Present");
     /* PDE present check for PTE */
     page_table = (pt_t *)( ((page_directory_entry->pd_base << 12)) + (pte*sizeof(int)) );
     //kprintf("\nPT BASE <%08x>",page_directory_entry->pd_base);
     //kprintf("\npage_table_entry: <%08x>:%d",page_table,page_table->pt_pres);
     //kprintf("\nPT_BASE <%08x>",page_table->pt_base);

     /* Check it was present should be present */
     if(page_table->pt_pres == 0)
     {
        //kprintf("\nPT absent");
        frm_no = get_frm(page_fr_no,FR_PAGE); 
        
        if(frm_no == SYSERR)
        {
           //restore(ps);
           return SYSERR;
        }
        /* We got a frame for page table entry now set its value */
        page_table->pt_pres = 1;
        page_table->pt_write = 1;
        page_table->pt_user = 0;
	page_table->pt_pwt = 0;
	page_table->pt_pcd = 0;
	page_table->pt_acc = 0;
	page_table->pt_dirty = 0;
	page_table->pt_mbz = 0;
	page_table->pt_global = 0;
	page_table->pt_avail = 0;
        page_table->pt_base = (FRAME0 + frm_no);
         
        bsm_lookup(currpid, page_fr_no, &store, &pageth);
        frm_tab[frm_no].fr_bs_id = store;
        frm_tab[frm_no].fr_pid = currpid;
        //kprintf("\npfint:<%d><%d><%d><%d><%d><%d>",frm_no,store,pageth,page_fr_no,frm_tab[frm_no].fr_pid,currpid);
        read_bs((char *)((FRAME0 + frm_no)*NBPG),store,pageth);
        //write_cr3(pdb*NBPG);
        //restore(ps);
        return OK;
     }
  }
  //restore(ps);
  //kprintf("Returning");
  return OK;
}
