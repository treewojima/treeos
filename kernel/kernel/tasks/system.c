/* treeos
 * Copyright (C) 2014-2016 Scott Bishop <treewojima@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <kernel/tasks/system.h>

#include <kernel/elf/elf32.h>
#include <kernel/interrupt.h>
#include <kernel/module.h>
#include <kernel/multiboot.h>
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

struct process g_kernel_system_task;

static void init_idle_task(void);
static void init_init_task(void);
static uint32_t get_init_entry(void);

void kernel_system_task(void)
{
    // Create the idle task
    init_idle_task();

    // Load init
    //init_init_task();
    get_init_entry();

    panic("lol");

    for (;;)
    {
        //bool lock = int_lock_region();
        printf("system task: ");
        dbg_print_esp();
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

#if 0
void init_init_task(void)
{
    bool lock = int_lock_region();

    struct process *task = kcalloc(1, sizeof(*task));

    task->pgdir = g_kernel_page_dir;
    task->quantum = DEFAULT_QUANTUM;
    task->flags |= PROC_FREE;

    task->thread = kcalloc(1, sizeof(struct thread));
    KASSERT(task->thread);
    task->thread->owner = task;

    uint32_t esp = KHEAP_START - PAGE_SIZE * 5;
    struct page_table_entry pte = { 0 };
    PANIC_IF(!page_alloc(&pte), "out of memory");
    page_map(pte.address << 12, esp - PAGE_SIZE, true, false);

    struct thread_context c = { 0 };

    c.eip = get_init_entry();
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

    scheduler_add_process(p);

    int_unlock_region(lock);
}
#endif

uint32_t get_init_entry(void)
{
    struct Elf32_Ehdr *hdr = get_module_by_name("init");
    KASSERT(hdr);

    for (uint32_t i = 0; i < hdr->e_phnum; i++)
    {
        struct Elf32_Phdr *prog_hdr = (struct Elf32_Phdr *)
                ((uint32_t)hdr + hdr->e_phoff + i * hdr->e_phentsize);

        if (prog_hdr->p_type != PT_LOAD) continue;


    }

    panic("lol");

    return 0;
}
