.section .text

.global sys_puts
.type sys_puts, @function
sys_puts:
   pushl %ebp
   movl  %esp, %ebp

   movl 8(%ebp), %ecx
   movl $1, %eax
   int  $0x30

   popl %ebp
   xorl %eax, %eax
   ret

