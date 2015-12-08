/* policy.c = srpolicy*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>


extern int page_replace_policy;
/*-------------------------------------------------------------------------
 * srpolicy - set page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL srpolicy(int policy)
{
  /* sanity check ! */

  kprintf("Setting the Page Replacement Policy!\n");
  if(policy == FIFO)
  {
     page_replace_policy = FIFO;
  }
  else if(policy == LRU)
  {
     page_replace_policy = LRU;
  }
  else
  {
     return SYSERR;
  }

  return OK;
}

/*-------------------------------------------------------------------------
 * grpolicy - get page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL grpolicy()
{
  return page_replace_policy;
}
