.section .text

.global scheduler_bootstrap_thread
.type scheduler_bootstrap_thread, @function
scheduler_bootstrap_thread:
    # Disable interrupts (will re-enable automatically)
    cli

    # This function will never return properly, so may as well wipe part of the
    # call stack frame to get to the function arguments
    addl $4  , %esp

    # Set g_current_process to the passed pointer
    movl (%esp)            , %eax
    movl $g_current_process, %edi
    movl %eax              , (%edi)

    # The first member of the process structure is the thread structure pointer
    movl (%eax), %ebp

    # Prepare the stack for the iret by reading from the thread's context
    # structure

    pushl 72(%ebp) # ss
    pushl 68(%ebp) # esp

    # Twiddle eflags to re-enable interrupts after iret
    pushfl
    popl   %eax
    orl    $0x200, %eax
    pushl  %eax

    pushl 60(%ebp) # cs
    pushl 56(%ebp) # eip

    # Jump into the thread
    iret
