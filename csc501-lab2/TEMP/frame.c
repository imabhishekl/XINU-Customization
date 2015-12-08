/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
#include <sleep.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
fr_map_t frm_tab[NFRAMES];
int page_replace_policy;
int fifo_count;
extern int it = 1;
SYSCALL init_frm()
{
  int i = 0;
  //kprintf("\nInit Page Frame!\n");
  for(i = 0;i < NFRAMES;i++)
  {
     frm_tab[i].fr_status = FRM_UNMAPPED;
     frm_tab[i].fr_pid = -1;
     frm_tab[i].fr_bs_id = -1;
     frm_tab[i].fr_vpno = -1;
     frm_tab[i].fr_loadtime = 0;
     frm_tab[i].fr_refcnt = 0;
     frm_tab[i].fr_type = -1;
  }
  return OK;
}

int free_frame(int id)
{
   frm_tab[id].fr_status = FRM_UNMAPPED;
   frm_tab[id].fr_pid = -1;
   frm_tab[id].fr_bs_id = -1;
   frm_tab[id].fr_vpno = -1;
   frm_tab[id].fr_loadtime = 0;
   frm_tab[id].fr_refcnt = 0;
   frm_tab[id].fr_type = -1;
   return OK;
}

long get_and_set_frame_for_pdb(int pid)
{
   int i = 0;
   pd_t *global_pd;
   int page_no = get_frm(0,FR_DIR);
   unsigned int pdb_addr = (FRAME0 + page_no)*NBPG;
   frm_tab[page_no].fr_pid = pid;
   pd_t *pde = (pd_t *)pdb_addr;
   global_pd = (pd_t *)proctab[0].pdbr;

   for(i = 0;i < NFRAMES;i++)
   {
      if(i < 4)
      {
         pde->pd_pres = 1;
         pde->pd_write = 1;
         pde->pd_base = (global_pd + i)->pd_base;
         pde = pde + 1;
      }
      else
      {
         pde->pd_pres = 0;
         pde = pde + 1;
      }
   }
   return pdb_addr;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int virt_page,int frame_type)
{
  int i,c;
  int frame_no;
  int found = 0;
  int min_load = 30000;
  int replace_id = -1;
  for(i = 5;i < NFRAMES;i++)
  {
     if(frm_tab[i].fr_type == FR_PAGE && frm_tab[i].fr_status == FRM_MAPPED)
     {
       update_LRU_count(frm_tab[i].fr_vpno,i);
       pd_t *page_directory_entry;
       pt_t *page_table,*temp;
       int frm_no;

       unsigned int pfa = frm_tab[i].fr_vpno; /* Read the page Fault address */
       unsigned int pdb = proctab[frm_tab[i].fr_pid].pdbr;//proctab[currpid].pdbr;  /* Read the PDBR of process caused fault */

       /* Get the PDE,PTE and Page Offset to replace the page */
       unsigned int pde = pfa >> 10;
       unsigned int pte = pfa & 0x000003FF;
       page_directory_entry = (pd_t *)(pdb + pde*sizeof(int)); /* Base + offset */

       page_table = (pt_t *)( ((page_directory_entry->pd_base * NBPG )) + (pte*sizeof(int)) );
       //kprintf("<%d>",page_table->pt_acc);
 
       if(page_table->pt_acc == 0)
       {
          if(frm_tab[i].fr_refcnt < min_load)
          {
             replace_id = i;
             //kprintf("<%d>",replace_id);
             min_load = frm_tab[i].fr_refcnt;
          }
       }
       //kprintf("\n");
       //update_LRU_count(frm_tab[i].fr_vpno,i);
     }

     if(frm_tab[i].fr_status == FRM_UNMAPPED && found == 0)
     {
        /* Found a Free Frame */
        frm_tab[i].fr_status = FRM_MAPPED;
        frm_tab[i].fr_type = frame_type;
        frm_tab[i].fr_pid = currpid;
        frm_tab[i].fr_vpno = virt_page;
        frm_tab[i].fr_loadtime = clktime;
        frm_tab[i].fr_refcnt = 1;
        //kprintf("F#<%d><%d>",i,frm_tab[i].fr_loadtime);
        //return i;
        frame_no = i;
        found = 1;
     }
  }

  if(found == 1)
  { 
     //kprintf("\nFound<%d>",frame_no);
     return frame_no;
  }

  /* Didnt found a free frame time to replace it */
  //kprintf("\nReplace<%d>",replace_id + FRAME0);
  frame_no = getReplaceFrame(replace_id);
  //kprintf("\nreplacing page<%d><%d>",(FRAME0 + frame_no),frm_tab[frame_no].fr_vpno,frm_tab[frame_no].fr_type);
  xmunmap(frm_tab[frame_no].fr_vpno);
  //free_frame(frame_no);
  frm_tab[frame_no].fr_status = FRM_MAPPED;
  frm_tab[frame_no].fr_type = frame_type;
  frm_tab[frame_no].fr_pid = currpid;
  frm_tab[frame_no].fr_vpno = virt_page;
  frm_tab[frame_no].fr_loadtime = clktime;

  //kprintf("\nRFound<%d><%d>",frame_no,min_load);
  return frame_no;
}

int getReplaceFrame(int replace_id)
{
   int i = 1;
   if(page_replace_policy == FIFO)
   {
     while(frm_tab[fifo_count].fr_type != FR_PAGE)
     {
        fifo_count++;
        //kprintf("FIFO<%d>",fifo_count);
        //i = fifo_count++;
        //fifo_count = (fifo_count%1024 == 0) ? 8:fifo_count%1024;
        if(fifo_count > NFRAMES)
           fifo_count = 8;
     }
     return fifo_count;
   }
   else if(page_replace_policy == LRU)
   {
      /* return the lowest time count */
      //if(it == 1)
      {
         it = 2;
         frm_tab[replace_id].fr_refcnt = 1;
         return replace_id;
      }
      /*else
      {
         if(replace_id == 6)
            replace_id = 6 + 340;
         frm_tab[replace_id].fr_refcnt = 1;
         return replace_id;
      }*/
   }
}

int get_proc_bs_id(int pid,int virtpage)
{
   int i;
   for(i = 0;i < TOTAL_BS;i++)
   {
      //if(pid == 47) kprintf("\nS<%d><%d><%d>",proctab[pid].bs_list[i].bs_vpno,proctab[pid].bs_list[i].bs_npages,virtpage);
    if((proctab[pid].bs_list[i].bs_vpno <= virtpage) && ((proctab[pid].bs_list[i].bs_vpno + proctab[pid].bs_list[i].bs_npages) >= virtpage))
//      if(proctab[pid].bs_list[i].bs_vpno == virtpage)
      {
         return i;
      }
   }
   return -1;
}

int write_back_data(int pid)
{
   int i = 0;
   int bs_id;
   int vpno;
//   kprintf("\nW-PID<%d>\n",pid);
   for(i = 0;i < NFRAMES;i++)
   {
      //if(i < 100)kprintf("\nW<%d><%d><%d><%d>",i,bs_id,frm_tab[i].fr_vpno,frm_tab[i].fr_pid);
      if(frm_tab[i].fr_pid == pid && frm_tab[i].fr_vpno > 4096 && frm_tab[i].fr_type == FR_PAGE )
      {
         bs_id = get_proc_bs_id(pid,frm_tab[i].fr_vpno);
         //kprintf("\nBSID<%d><%d><%d>",bs_id,pid,frm_tab[i].fr_vpno);
         vpno = frm_tab[i].fr_vpno;
         //kprintf("\nW<%d><%d><%d><%d><%d>",i,bs_id,vpno,pid,frm_tab[i].fr_pid);
         if(bs_id != -1)
         {
            //kprintf("\nBS-ID<%d><%d><%d>",bs_id,i,(vpno - proctab[pid].bs_list[bs_id].bs_vpno));
            //kprintf("\nFound BS<%d> for write back<%d><%d><%d><%d>",bs_id,pid,vpno,i,(vpno - proctab[pid].bs_list[bs_id].bs_vpno));
            write_bs((char *)((i + FRAME0)*NBPG),bs_id,(vpno - proctab[pid].bs_list[bs_id].bs_vpno));
     //       kprintf("\nAfter Write <%c>",(char *)((i + FRAME0)*NBPG));
         }
         else
         {
            //kprintf("\nDidnt Found Page");
         }
      }
   }   
   //kprintf("\nReturning from write_back_data");
   return OK;
}

int read_back_data(int pid)
{
   int i = 0;
   int bs_id;
   int vpno;
   //kprintf("\nR-PID<%d>",pid);
   for(i = 0;i < NFRAMES;i++)
   {
      if(frm_tab[i].fr_pid == pid && frm_tab[i].fr_vpno > 4096 && frm_tab[i].fr_type == FR_PAGE)
      {
         bs_id = get_proc_bs_id(pid,frm_tab[i].fr_vpno);
         //kprintf("\nBSID<%d><%d><%d>",bs_id,pid,frm_tab[i].fr_vpno);
         vpno = frm_tab[i].fr_vpno;
         //kprintf("\nR<%d><%d><%d><%d><%d>",i,bs_id,vpno,pid,frm_tab[i].fr_pid);
         if(bs_id != -1)
         {
            //kprintf("\nBS-ID<%d><%d><%d>",bs_id,i,(vpno - proctab[pid].bs_list[bs_id].bs_vpno));
            //kprintf("\nFound BS<%d> for read back<%d><%d><%d><%d>",bs_id,pid,vpno,i,(vpno - proctab[pid].bs_list[bs_id].bs_vpno));
            read_bs((char *)((i + FRAME0)*NBPG),bs_id,(vpno - proctab[pid].bs_list[bs_id].bs_vpno));
         }
         else
         {
            //kprintf("\nDidnt Found Page");
         }
      }
   }
   //kprintf("\nReturning from read_back_data");
   return OK;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm_kill(int pid)
{
  int i;
  //kprintf("In free_frm!\n");

  for(i = 0;i < NFRAMES;i++)
  {
     //kprintf("Release Frame <%d><%d>",frm_tab[i].fr_pid,pid);
     if(frm_tab[i].fr_pid == pid)
     {
        //kprintf("\nRelease Frame <%d>",i);
        frm_tab[i].fr_status = FRM_UNMAPPED;
        frm_tab[i].fr_pid = -1;
        frm_tab[i].fr_bs_id = -1;
        frm_tab[i].fr_loadtime = 0;
        frm_tab[i].fr_refcnt = 0;
        frm_tab[i].fr_type = -1;
     }
  }

  return OK;
}
 
SYSCALL free_frm(int bs_id,int pid)
{
  int i;
  //kprintf("In free_frm!\n");

  for(i = 0;i < NFRAMES;i++)
  {
     if(frm_tab[i].fr_bs_id == bs_id && frm_tab[i].fr_pid == pid && frm_tab[i].fr_type == FR_PAGE)
     {
        frm_tab[i].fr_status = FRM_UNMAPPED;
        frm_tab[i].fr_pid = -1;
        frm_tab[i].fr_bs_id = -1;
        frm_tab[i].fr_loadtime = 0;
        frm_tab[i].fr_refcnt = 0;
        frm_tab[i].fr_type = -1;
     }
  }

  return OK;
}

int update_LRU_count(int virt_page,int frame_index)
{
  pd_t *page_directory_entry;
  pt_t *page_table,*temp;
  int frm_no;
  int k;

  //disable(ps);
  
  unsigned int pfa = virt_page; /* Read the page Fault address */
  unsigned int pdb = proctab[frm_tab[frame_index].fr_pid].pdbr;//proctab[currpid].pdbr;  /* Read the PDBR of process caused fault */
  //kprintf("\nPage Fault Address Generated is <%08x><%d>",pfa,pfa,page_fr_no);

  /* Get the PDE,PTE and Page Offset to replace the page */
  unsigned int pde = pfa >> 10;
  unsigned int pte = pfa & 0x000003FF;
  //kprintf("\nPDB <%08x>",pdb);
  page_directory_entry = (pd_t *)(pdb + pde*sizeof(int)); /* Base + offset */

  page_table = (pt_t *)( ((page_directory_entry->pd_base * NBPG )) + (pte*sizeof(int)) );
  //kprintf("\n<%d><%d>",page_table->pt_acc,frame_index);
  if( page_table->pt_acc == 1)
  {
     //kprintf("\nFrame<%d><%d> was accessed",frame_index,frm_tab[frame_index].fr_refcnt);
     frm_tab[frame_index].fr_refcnt = frm_tab[frame_index].fr_refcnt + 1;
     //kprintf("Frame<%d><%d> was accessed",frame_index,frm_tab[frame_index].fr_refcnt);
     page_table->pt_acc = 0;
  }
}
