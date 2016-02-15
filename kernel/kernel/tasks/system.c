#include <kernel/tasks/system.h>

#include <kernel/proc/process.h>
#include <kernel/interrupt.h>
#include <stdio.h>

struct process g_kernel_system_task;

void kernel_system_task(void)
{
    // TODO: Use the system task to bootstrap the idle task with a valid
    //       thread context

    for (;;)
    {
        //bool lock = int_lock_region();
        printf("system task\n");
        //int_unlock_region(lock);
        process_yield();
    }
}
