#include <kernel/proc/process.h>

#include <stdlib.h>

struct process *process_create(struct process *process)
{
    if (!process)
    {
        process = kcalloc(1, sizeof(*process));
        process->flags |= PROC_FREE;
    }

    return process;
}
