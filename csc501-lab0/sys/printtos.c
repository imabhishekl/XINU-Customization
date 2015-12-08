#include <stdio.h>

static unsigned long *ebp,*esp;

void printtos()
{
    asm("movl %ebp,ebp");
    asm("mov  %esp,esp");
    int lv1 = 1,lv2 = 2,lv3 = 3,i = 0;
 
    kprintf("\n\nRun Time Stack Pointer : 0x%08x",esp);
    kprintf("\nCurrent Base Pointer : 0x%08x",ebp);
    kprintf("\nPrevious Stack Pointer : 0x%08x\n",(ebp + 2));
   
    kprintf("\nPrinting the recent 6 value pushed in stack");
    for(i = 0; i < 6 ; i++)
    {
       kprintf("\n#%d:Address[0x%08x]: 0x%08x", i,esp + i, *(esp + i));  
    }
}
