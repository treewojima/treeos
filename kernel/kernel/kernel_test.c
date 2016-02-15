#include "kernel_test.h"

#include <arch/i386/cpu.h>
#include <kernel/debug.h>
#include <kernel/proc/scheduler.h>
#include <kernel/tasks/idle.h>
#include <kernel/tasks/system.h>
#include <kernel/vmm/heap.h>
#include <stdlib.h>
#include <string.h>

extern struct page_dir_entry *g_kernel_page_dir;

static void init_idle_task(void);
static void init_system_task(void);

void test_tasks(void)
{
    init_system_task();
    init_idle_task();

    scheduler_bootstrap_thread(&g_kernel_system_task);
}

void init_idle_task(void)
{
    struct process *task = &g_kernel_idle_task;
    memset(task, 0, sizeof(*task));

    task->pgdir = g_kernel_page_dir;
    task->quantum = DEFAULT_QUANTUM;
    //task->next = &g_kernel_system_task;

    task->thread = kcalloc(1, sizeof(struct thread));
    KASSERT(task->thread);
    task->thread->owner = task;

    uint32_t esp = KHEAP_START - PAGE_SIZE;
    struct page_table_entry pte = { 0 };
    PANIC_IF(!page_alloc(&pte), "out of memory");
    page_map(pte.address << 12, esp - PAGE_SIZE, true, false);

    struct thread_context *c = &task->thread->context;
    c->esp = esp;
    c->eip = (uint32_t)&kernel_idle_task;
    c->cs = 0x8;
    c->ss = c->ds = c->es = c->fs = c->gs = 0x10;
    c->eflags = read_eflags() | 0x200;

    scheduler_add_process(task);
}

void init_system_task(void)
{
    struct process *task = &g_kernel_system_task;
    memset(task, 0, sizeof(*task));

    task->pgdir = g_kernel_page_dir;
    task->quantum = DEFAULT_QUANTUM;
    //task->next = &g_kernel_idle_task;

    task->thread = kcalloc(1, sizeof(struct thread));
    KASSERT(task->thread);
    task->thread->owner = task;

    uint32_t esp = KHEAP_START - (PAGE_SIZE * 3);
    struct page_table_entry pte = { 0 };
    PANIC_IF(!page_alloc(&pte), "out of memory");
    page_map(pte.address << 12, esp - PAGE_SIZE, true, false);

    struct thread_context *c = &task->thread->context;
    c->esp = esp;
    c->eip = (uint32_t)&kernel_system_task;
    c->cs = 0x8;
    c->ss = c->ds = c->es = c->fs = c->gs = 0x10;
    c->eflags = read_eflags() | 0x200;

    scheduler_add_process(task);
}
