#include <stdio.h>

/* This are the global variable declared in xinu for segments details */
extern int etext,edata,end; /* Data denoting text,data and bss segment */

int printsegaddress();

int printsegaddress()
{
   /* Print this value */
   kprintf("\n\nPreceding Segment [Address]:Value");
   kprintf("\nText [0x%08x]:0x%08x ",&etext - 1,*(&etext - 1));
   kprintf("\nData [0x%08x]:0x%08x ",&edata - 1,*(&edata - 1));
   kprintf("\nBSS  [0x%08x]:0x%08x ",&end - 1,*(&end - 1));

   kprintf("\n\nCurrent Segment [Address]:Value");
   kprintf("\nText [0x%08x]:0x%08x ",&etext,etext);
   kprintf("\nData [0x%08x]:0x%08x ",&edata,edata);
   kprintf("\nBSS  [0x%08x]:0x%08x ",&end,end);

   kprintf("\n\nAfter Segment [Address]:Value");
   kprintf("\nText [0x%08x]:0x%08x ",&etext + 1,*(&etext + 1));
   kprintf("\nData [0x%08x]:0x%08x ",&edata + 1,*(&edata + 1));
   kprintf("\nBSS  [0x%08x]:0x%08x ",&end + 1,*(&end + 1));


   return 0;
}
