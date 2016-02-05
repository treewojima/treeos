.section .text

.global _start
.type _start, @function
_start:
    movl $0xDEADBEEF, %eax
.L:
    hlt
    jmp .L

