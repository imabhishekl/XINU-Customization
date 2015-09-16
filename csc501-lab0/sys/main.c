/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

void halt();

unsigned long *ebp_i;

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int main()
{
        //syscallsummary_start();
	kprintf("\n\nHello World, Xinu lives\n");
        long param = 0xaabbccdd;
        getpid();
        kprintf("\nBefore : 0x%08x",param);
        kprintf("\nAfter  : 0x%08x",zfunction(param));
        printsegaddress();
        asm("movl %esp,ebp_i");
        kprintf("\n<0x%08x><0x%08x>",ebp_i,*ebp_i);
        printtos();
        printprocstks(100);
        printsyscalls();
	return 0;
}
