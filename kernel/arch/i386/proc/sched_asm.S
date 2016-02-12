.section .text

.global sched_bootstrap_thread
.type sched_bootstrap_thread, @function
sched_bootstrap_thread:
    # Disable interrupts (will re-enable automatically)
    sti

    # This function will never return properly, so just wipe part of the
    # call stack frame to get to the function arguments
    addl $4  , %esp
    movl %esp, %ebp

    # Stack segment selector
    pushl $0x10

    # New stack pointer
    movl  (%ebp), %eax
    pushl %eax

    # Twiddle eflags to re-enable interrupts after iret
    pushfl
    popl   %eax
    orl    $0x200, %eax
    pushl  %eax

    # Code segment selector
    pushl $0x8

    # New EIP
    pushl 4(%ebp)

    # Jump into the thread
    iret
