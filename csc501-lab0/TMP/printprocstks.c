#include <stdio.h>
#include <kernel.h>
#include <proc.h>

unsigned long *esp = NULL;

void printprocstks(int priority)
{
   int i = 0;
   /* Loop on the process list */
   for(i = 0 ; i < NPROC ; i++)
   {
      if(proctab[i].pstate == PRFREE) /* Check the proces state */
         continue;

      if(proctab[i].pprio < priority) /* Priority of process is less */
      {
         /* Print the Stack base,Stack size,Stack limit and Stack pointer */
         kprintf("\n*****************************************************");
         kprintf("\nProcess Name      : <%16s>",proctab[i].pname);
         kprintf("\nProcess Id        : <%d>  ",i);
         kprintf("\nProcess Priority  : <%d>  ",proctab[i].pprio);
         kprintf("\nStack Base        : <0x%08x>",proctab[i].pbase);
         kprintf("\nStack Size        : <%d>",proctab[i].pstklen);
         kprintf("\nStack Limit       : <0x%08x>",proctab[i].plimit);
         if(i == currpid)
         {
            asm("movl %esp, esp");
            kprintf("\nStack Pointer     : <0x%08x>  ",esp);
         }
         else
         {
            kprintf("\nStack Pointer     : <0x%08x>  ",proctab[i].pesp);
         }
         kprintf("\n*****************************************************");
      }
   }
}
