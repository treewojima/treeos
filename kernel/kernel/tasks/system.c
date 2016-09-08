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

#include <arch/i386/pmm/allocator.h>
#include <kernel/elf/elf32.h>
#include <kernel/interrupt.h>
#include <kernel/module.h>
#include <kernel/multiboot.h>
#include <kernel/proc/process.h>
#include <kernel/proc/scheduler.h>
#include <kernel/tasks/idle.h>
#include <kernel/vmm/addr.h>
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
static struct page_dir_entry *clone_page_dir(struct page_dir_entry *pde);

void kernel_system_task(void)
{
    // Create the idle task
    init_idle_task();

    // Load init
    //init_init_task();
    get_init_entry();
    struct page_dir_entry *pde = clone_page_dir(NULL);
    KASSERT(pde);

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

void init_init_task(void)
{
    bool lock = int_lock_region();

    struct process *task = kcalloc(1, sizeof(struct process));
    KASSERT(task);
    task->flags |= PROC_FREE;

    // Create a new page directory for the process
    task->pgdir = clone_page_dir(NULL);
    task->quantum = DEFAULT_QUANTUM;

    task->thread = kcalloc(1, sizeof(struct thread));
    KASSERT(task->thread);
    task->thread->owner = task;

    // Calculate how much memory is required

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

uint32_t get_init_entry(void)
{
    struct Elf32_Ehdr *hdr = get_module_by_name("init");
    KASSERT(hdr);

    // First, loop through the program headers to validate and calculate
    // the base address
    Elf32_Word base = -1;
    for (uint32_t i = 0; i < hdr->e_phnum; i++)
    {
        struct Elf32_Phdr *prog_hdr = (struct Elf32_Phdr *)
                ((uint32_t)hdr + hdr->e_phoff + i * hdr->e_phentsize);

        if (prog_hdr->p_type != PT_LOAD) continue;

        // Base address is the smallest p_vaddr entry
        if (prog_hdr->p_vaddr < base) base = prog_hdr->p_vaddr;
    }

    // Loop through them again to calculate the amount of memory required
    Elf32_Word size = 0;
    for (uint32_t i = 0; i < hdr->e_phnum; i++)
    {
        struct Elf32_Phdr *prog_hdr = (struct Elf32_Phdr *)
                ((uint32_t)hdr + hdr->e_phoff + i * hdr->e_phentsize);

        // Size is the largest p_vaddr entry plus p_memsz
        Elf32_Word s = prog_hdr->p_paddr - base + prog_hdr->p_memsz;
        if (s > size) size = s;
    }

    printf("init base: %x\n", base);
    printf("init size: %x\n", size);
    //panic("lol");

    return 0;
}

struct page_dir_entry *clone_page_dir(struct page_dir_entry *pde)
{
    if (!pde)
    {
#if 0
        // Allocate a chunk of memory for a page directory
        struct page_table_entry pte = { 0 };
        PANIC_IF(!page_alloc(&pte), "out of memory");
        pde = (struct page_dir_entry *)(pte.address << 12);
#else
        pde = paging_structure_alloc();
#endif
    }
    KASSERT(IS_PAGE_ALIGNED(pde));

    // Copy kernel pages into new directory
    // NOTE: for now, just blindly copy everything above the higher half page
    memcpy(pde + 768,
           g_kernel_page_dir + 768,
           255);
    pde[1023].present = true;
    pde[1023].rw = true;
    pde[1023].address = (uint32_t)virt_to_phys(pde) >> 12;

    return pde;
}
