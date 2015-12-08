.file   "zfunction.S"

.globl zfunction
zfunction:
pushl   %ebp
movl    %esp,%ebp
addl    $8,%esp
movl    (%esp),%eax
and     $0xFFFFF00F,%eax
shl     $0x08,%eax
leave
ret
