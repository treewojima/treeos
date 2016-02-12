#include <arch/i386/cpu.h>
#include <arch/i386/serio.h>
#include <kernel/interrupt.h>
#include <kernel/multiboot.h>
#include <kernel/panic.h>
#include <kernel/proc/sched.h>
#include <kernel/pmm.h>
#include <kernel/timer.h>
#include <kernel/tty.h>
#include <kernel/vmm/addr.h>
#include <kernel/vmm/heap.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kernel_test.h"

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
    extern const struct multiboot_info g_multiboot_info;
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
 *     void
 */
void kmain(void)
{
    // Load our GDT and initialize interrupts
    gdt_init();
    int_init();
    tss_init();

    // Initialize system timer and keyboard driver
    timer_init(DEFAULT_SYSTEM_TIMER_FREQ);
    //kbd_init();

    // Initialize physical and virtual memory
    pmm_init();
    int_enable();

#ifdef TEST
    test_map_page();
    test_malloc();
#endif
    sched_init();
    test_usermode_stage1();

    panic("end of kmain");
}
