#include <kernel/tasks/system.h>

#include <kernel/interrupt.h>
#include <kernel/proc/process.h>
#include <kernel/proc/scheduler.h>
#include <kernel/tasks/idle.h>
#include <kernel/vmm/heap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PUSH(context, type, value)           \
    do {                                     \
        (context)->esp -= sizeof(type);      \
        *((type *)(context)->esp) = (value); \
    } while (0)

extern struct page_dir_entry *g_kernel_page_dir;

struct process g_kernel_system_task;

static void init_idle_task(void);

void kernel_system_task(void)
{
    // Create the idle task
    init_idle_task();

    for (;;)
    {
        //bool lock = int_lock_region();
        printf("system task\n");
        //int_unlock_region(lock);
        process_yield();
    }
}

void init_idle_task(void)
{
    bool lock = int_lock_region();

    struct process *task = &g_kernel_idle_task;
    memset(task, 0, sizeof(*task));

    task->pgdir = g_kernel_page_dir;
    task->quantum = DEFAULT_QUANTUM;

    task->thread = kcalloc(1, sizeof(struct thread));
    KASSERT(task->thread);
    task->thread->owner = task;

    uint32_t esp = KHEAP_START - PAGE_SIZE;
    struct page_table_entry pte = { 0 };
    PANIC_IF(!page_alloc(&pte), "out of memory");
    page_map(pte.address << 12, esp - PAGE_SIZE, true, false);

    //struct thread_context *c = kcalloc(1, sizeof(*c));
    //task->flags |= PROC_FREE_THREAD_CONTEXT;
    //task->thread->context = c;

    struct thread_context c = { 0 };

    //c->esp = esp;
    c.eip = (uint32_t)&kernel_idle_task;
    c.cs = 0x8;
    c.ds = c.es = c.fs = c.gs = 0x10;
    c.eflags = read_eflags() | 0x200;

    // Pad the stack with some expected values for the interrupt handler,
    // so that it looks like the the thread was interrupted and will resume
    // right where it's supposed to start
    esp -= sizeof(c) - 8;
    c.esp = esp + THREAD_CONTEXT_OFFSET;
    memcpy((void *)esp, &c, sizeof(c) - 8);
    task->thread->context = (struct thread_context *)esp;

    scheduler_add_process(task);

    int_unlock_region(lock);
}
