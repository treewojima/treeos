.section .text

.global test_usermode
.type test_usermode, @function
test_usermode:
    movw $0x23, %ax
    movw %ax  , %ds
    movw %ax  , %es
    movw %ax  , %fs
    movw %ax  , %gs

    movl  %esp, %eax
    pushl $0x23
    pushl %eax
    pushfl
    pushl $0x1B
    pushl $test_usermode_c
    iret

.global sys_puts
.type sys_puts, @function
sys_puts:
    pushl %ebp
    movl  %esp, %ebp

    pushl $0xCAFEBABE

    movl $0x1   , %ecx
    movl 8(%ebp), %edx
    int  $0x30

    addl $4, %esp

    movl %ebp, %esp
    popl %ebp
    ret
