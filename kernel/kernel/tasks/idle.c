#include <kernel/tasks/idle.h>

#include <kernel/interrupt.h>
#include <kernel/proc/process.h>
#include <stdio.h>

struct process g_kernel_idle_task;

void kernel_idle_task(void)
{
    for (;;)
    {
        //bool lock = int_lock_region();
        printf("idle task\n");
        //int_unlock_region(lock);
        process_yield();
    }
}
