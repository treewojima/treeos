#include <arch/i386/proc/thread.h>

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
    movl %eax              , g_current_process

    # Grab a pointer to the thread context
    movl (%eax), %edx
    movl (%edx), %ebp

    # Since we're bootstrapping a kernel thread, iret won't pop ss/esp off the
    # stack - so just move the stack pointer to where the new stack will be
    movl 28(%ebp), %esp

    # Twiddle eflags to re-enable interrupts after iret
    pushfl
    popl   %eax
    orl    $0x200, %eax
    pushl  %eax

    pushl 60(%ebp) # cs
    pushl 56(%ebp) # eip

    # Jump into the thread
    iret

.global scheduler_advance_stage2
.type scheduler_advance_stage2, @function
scheduler_advance_stage2:
    # Set the stack pointer to the new process's stack
    movl g_current_process, %eax
    movl (%eax)           , %edx

    # All this:
    #movl (%edx)           , %ebp
    #movl THREAD_CONTEXT_OFFSET(%ebp), %esp
    # esp is the original value, so adjust it for the rest of the
    # thread context
    #subl $THREAD_CONTEXT_OFFSET, %esp

    # Has the same effect as this:
    movl (%edx), %esp

    movl $0, g_switch_processes

    jmp int_handler_stub_resume
