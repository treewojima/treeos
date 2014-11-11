.section .text

#include <arch/i386/interrupt.h>
#include <kernel/syscall.h>

# Wrapper function that triggers a syscall interrupt
.global syscall
.type syscall, @function
syscall:
    movl 4(%esp), %eax
    int $INT_SYSCALL_INTERRUPT
    ret
