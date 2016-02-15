#include <kernel/proc/process.h>

#include <kernel/interrupt.h>
#include <stdlib.h>

extern struct process *g_current_process;

struct process *process_create(struct process *process)
{
    if (!process)
    {
        process = kcalloc(1, sizeof(*process));
        process->flags |= PROC_FREE;
    }

    return process;
}

void process_yield(void)
{
    bool lock = int_lock_region();
    g_current_process->flags |= PROC_DEBUG_YIELD;
    int_unlock_region(lock);
}
