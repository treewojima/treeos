.section .text

.global test_usermode
.type test_usermode, @function
test_usermode:
    movw $0x23, %ax
    movw %ax  , %ds
    movw %ax  , %es
    movw %ax  , %fs
    movw %ax  , %gs

    pushl $0xDEADBEEF

    movl  %esp, %eax
    pushl $0x23
    pushl %eax
    pushfl
    pushl $0x1B
    pushl $test_usermode_c
    iret

.global test_usermode_2
.type test_usermode_2, @function
test_usermode_2:
    popl %eax
.L:
    hlt
    jmp .L

.global sys_puts
.type sys_puts, @function
sys_puts:
    pushl %ebp
    movl  %esp, %ebp

    movl $0x1   , %ecx
    movl 8(%ebp), %edx
    int  $0x30

    movl %ebp, %esp
    popl %ebp
    ret
