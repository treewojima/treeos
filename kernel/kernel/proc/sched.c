#include <kernel/proc/sched.h>

#include <kernel/panic.h>
#include <kernel/proc/process.h>
#include <kernel/reboot.h>
#include <kernel/vmm/heap.h> // for kheap boundaries
#include <stdio.h>
#include <string.h>

#ifdef TREEOS_I386
extern void sched_bootstrap_thread(uint32_t esp, uint32_t eip);
#endif

static void create_kernel_process(void);
static void kernel_idle_thread_function(void);

static struct process kernel_idle_process;
static struct thread kernel_idle_thread;

void sched_init(void)
{
    create_kernel_process();
    sched_bootstrap_thread(kernel_idle_thread.registers.esp,
                           kernel_idle_thread.registers.eip);
}

void create_kernel_process(void)
{
    //kernel_process = kcalloc(1, sizeof(*kernel_process));
    memset(&kernel_idle_process, 0, sizeof(kernel_idle_process));
    memset(&kernel_idle_thread, 0, sizeof(kernel_idle_thread));

    // Since this is a kernel thread, use the kernel page directory
    extern struct page_dir_entry *g_kernel_page_dir;
    kernel_idle_process.pgdir = g_kernel_page_dir;

    //kernel_process->thread = kcalloc(1, sizeof(*kernel_process->thread));
    kernel_idle_process.thread = &kernel_idle_thread;
    kernel_idle_thread.owner = &kernel_idle_process;

    struct registers *r = &kernel_idle_thread.registers;

    // Code and data segments
    r->cs = 0x8;
    r->ds = 0x10;
    r->es = 0x10;
    r->fs = 0x10;
    r->gs = 0x10;
    r->ss = 0x10;

    // Set up a new stack in a safe location (leave a blank, unmapped page
    // between this and the heap to prevent overruns
    r->esp = KHEAP_START - PAGE_SIZE;
    struct page_table_entry pte = { 0 };
    PANIC_IF(!page_alloc(&pte), "out of memory");
    page_map(pte.address << 12, r->esp - PAGE_SIZE, true, false);

    // Is this really necessary?
    r->eflags = read_eflags();

    // Set instruction pointer to the address of the idle function
    r->eip = (uint32_t)&kernel_idle_thread_function;
}

void kernel_idle_thread_function(void)
{
    printf("lololol\n");
    halt();
}
