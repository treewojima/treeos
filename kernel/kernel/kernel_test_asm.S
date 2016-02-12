.section .text

.global test_usermode_stage2
.type test_usermode_stage2, @function
test_usermode_stage2:
    movw $0x23, %ax
    movw %ax  , %ds
    movw %ax  , %es
    movw %ax  , %fs
    movw %ax  , %gs

    #movl  %esp, %eax
    pushl $0x23
    #pushl %eax
    movl $g_usermode_esp, %eax
    pushl (%eax)
    pushfl
    pushl $0x1B
    pushl $test_usermode_stage3
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
