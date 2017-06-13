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

#include <arch/i386/cpu.h>
#include <arch/i386/serio.h>
#include <kernel/elf/elf32.h>
#include <kernel/interrupt.h>
#include <kernel/module.h>
#include <kernel/multiboot.h>
#include <kernel/panic.h>
#include <kernel/proc/scheduler.h>
#include <kernel/pmm.h>
#include <kernel/timer.h>
#include <kernel/tty.h>
#include <kernel/vmm/addr.h>
#include <kernel/vmm/heap.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Stack manipulation macro for raw memory locations
#define PUSH(context, type, value)           \
    do {                                     \
        (context)->esp -= sizeof(type);      \
        *((type *)(context)->esp) = (value); \
    } while (0)

// *Virtual* address of kernel stack, for use when creating new threads
extern char g_kernel_stack_bottom;

// Kernel tasks and threads
enum
{
    SYSTEM,
    INIT,
    NUM_TASKS
};
static struct process kernel_tasks[NUM_TASKS];
static struct thread kernel_threads[NUM_TASKS];

//void kmain(void) NORETURN;
static void NORETURN init_system_task(void);
static void NORETURN kernel_system_task(void);
static void spawn_init(void);
static void NORETURN kernel_idle_task(void);

/* Kernel C-code entry point
 * Parameters:
 *     boot_magic    - magic number passed by a Multiboot-compatible bootloader
 *     boot_record   - Multiboot details, such as module addresses
 *
 * Returns:
 *     void
 */
void kinit(uint32_t boot_magic,
           struct multiboot_info *mbi)
{
    // Panic if either the Multiboot magic number is wrong, or if there's no
    // Multiboot info record (this is done after
    PANIC_IF(boot_magic != MULTIBOOT_BOOTLOADER_MAGIC,
             "kinit not called with correct multiboot magic");
    PANIC_IF(mbi == NULL,
             "NULL multiboot info record passed to kinit");

    // Copy the Multiboot info struct so it doesn't get overwritten
    // in memory later
    memcpy((struct multiboot_info *)&g_multiboot_info, mbi, sizeof(*mbi));

    // Initialize the (very primitive) serial and terminal drivers
    serio_init(SERIO_COM1);
    tty_init();

    // Initialize the kernel placement heap
    kernel_heap_init();
}

/* Kernel C-code main function, called once global constructors/initializers
 * have been called.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     never
 */
void NORETURN kmain(void)
{
    // Load our GDT and initialize interrupts
    gdt_init();
    int_init();
    tss_init();

    // Initialize physical memory allocator
    pmm_init();

    // Remap memory-mapped I/O addresses into high kernel space
    tty_remap();

    // Prepare the kernel module table
    module_table_init();

    // Initialize system timer and keyboard driver
    timer_init(DEFAULT_SYSTEM_TIMER_FREQ);
    //kbd_init();

    // Initialize the scheduling system
    scheduler_init();

    // Initialize the main task and start the rest of the system
    init_system_task();
}

// TODO: try and refactor this to reduce code duplication when spinning up
//       tasks/processes and threads
void NORETURN init_system_task(void)
{
    memset(kernel_tasks, 0, sizeof(struct process) * NUM_TASKS);
    struct process *task = &kernel_tasks[SYSTEM];

    // The kernel from this point on becomes the system task, so use our
    // existing page directory
    task->pgdir = (struct page_dir_entry *)0xFFFFF000; //g_kernel_page_dir;
    task->quantum = DEFAULT_QUANTUM;

    // A thread structure is required for context switching
    memset(kernel_threads, 0, sizeof(struct thread) * NUM_TASKS);
    task->thread = &kernel_threads[SYSTEM];
    task->thread->owner = task;

    // Create a temporary thread context
    struct thread_context c;
    task->thread->context = &c;

    // Since we're never going back, we can safely reuse (and thus mangle)
    // the existing kernel stack
    c.esp = (uint32_t)&g_kernel_stack_bottom;

    // Set entry point to the kernel_system_task function
    c.eip = (uint32_t)&kernel_system_task;

    // Selectors and flags
    c.cs = 0x8;
    c.ds = c.es = c.fs = c.gs = 0x10;
    c.eflags = read_eflags() | 0x200;

    // Add process to scheduler and "activate" it
    scheduler_add_process(task);
    scheduler_bootstrap_thread(&kernel_tasks[SYSTEM]);
}

void NORETURN kernel_system_task(void)
{
    // Load init, which in turn continues to load the rest of the system
    spawn_init();

    // Spin off into the idle task
    for (;;)
    {
        //bool lock = int_lock_region();
        printf("system task: ");
        dbg_print_esp();
        //int_unlock_region(lock);
        process_yield();
        while (kernel_tasks[SYSTEM].flags & PROC_DEBUG_YIELD)
        {
            asm_hlt();
        }
    }
}

void spawn_init(void)
{
    bool lock = int_lock_region();

    struct process *task = &kernel_tasks[INIT];

    // LOOK AT THIS:
    // https://github.com/stevej/osdev/blob/master/kernel/sys/task.c

    // Clone the kernel's page directory
    // NOTE: for now, just blindly copy everything above the higher half page,
    //       since in theory that's all any new process will need
    task->pgdir = paging_structure_alloc();
    memcpy(task->pgdir + 768,
           g_kernel_page_dir + 768,
           255);
    task->pgdir[1023].present = true;
    task->pgdir[1023].rw = true;
    task->pgdir[1023].address = (uint32_t)virt_to_phys(task->pgdir) >> 12;

    // Switch to the "new" address space to make page table manipulation easier
    //write_cr3((uint32_t)virt_to_phys(task->pgdir));
    //write_cr0(read_cr0() | 0x80000000);

    task->quantum = DEFAULT_QUANTUM;
    task->thread = &kernel_threads[INIT];
    task->thread->owner = task;

    // Grab a blank page for use as a stack and stick it somewhere
    // NOTE: this location is arbitrary, for now
    uint32_t esp = KHEAP_START - PAGE_SIZE;
    struct page_table_entry pte = { 0 };
    PANIC_IF(!page_alloc(&pte), "out of memory");
    page_map(pte.address << 12, esp - PAGE_SIZE, true, false);

    // TODO: make this a load_process trampoline function
    struct thread_context c = { 0 };
    c.eip = (uint32_t)&kernel_idle_task;
    c.cs = 0x8;
    c.ds = c.es = c.fs = c.gs = 0x10;
    c.eflags = read_eflags() | 0x200;

    // Pad the stack with some expected values for the interrupt handler,
    // so that it looks like the the thread was interrupted and will resume
    // right where it's supposed to start
    esp -= sizeof(c) - 8;
    c.esp = esp + THREAD_CONTEXT_OFFSET;
    //int_unlock_region(lock);
    memcpy((void *)esp, &c, sizeof(c) - 8);
    //lock = int_lock_region();
    task->thread->context = (struct thread_context *)esp;

    //panic("fuck this shit");
    int_unlock_region(lock);

    scheduler_add_process(task);
}

void NORETURN kernel_idle_task(void)
{
    for (;;)
    {
        printf("idle task: ");
        dbg_print_esp();
        process_yield();
        while (kernel_tasks[INIT].flags & PROC_DEBUG_YIELD)
        {
            asm_hlt();
        }
    }
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

        // Copy kernel pages into new directory
        // NOTE: for now, just blindly copy everything above the higher half page
        memcpy(pde + 768,
               g_kernel_page_dir + 768,
               255);
        pde[1023].present = true;
        pde[1023].rw = true;
        pde[1023].address = (uint32_t)virt_to_phys(pde) >> 12;
    }

    KASSERT(IS_PAGE_ALIGNED(pde));
    return pde;
}
