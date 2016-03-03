#include <kernel/tasks/idle.h>

#include <kernel/interrupt.h>
#include <kernel/proc/process.h>
#include <stdio.h>

struct process g_kernel_idle_task;

void kernel_idle_task(void)
{
    for (;;)
    {
        process_yield();
        while (g_kernel_idle_task.flags & PROC_DEBUG_YIELD)
        {
            asm volatile("hlt": : : "memory");
        }
    }
}
