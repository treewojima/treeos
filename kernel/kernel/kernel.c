#include <arch/i386/cpu.h>
#include <arch/i386/interrupt.h>
#include <kernel/multiboot.h>
#include <kernel/panic.h>
#include <kernel/timer.h>
#include <kernel/tty.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Reference to the global initial stack pointer
uint32_t g_initial_stack;

void kinit(void)
{
    // Clear the .bss (GRUB is supposed to do this for us, but don't count
    // on it)
    extern uint8_t g_bss_start, g_bss_end;
    size_t count = &g_bss_end - &g_bss_start;
    memset(&g_bss_start, 0, count);

    // Initialize our first-stage stack and heap
    //g_initial_stack = initial_stack;
    //extern uint8_t g_
    //kheap_init(boot_record);

    // Initialize the (very primitive) terminal driver
    tty_init();
}

/* Kernel C-code entry point
 * Parameters:
 *     boot_magic    - magic number passed by a Multiboot-compatible bootloader
 *     boot_record   - Multiboot details, such as module addresses
 *
 * Returns:
 *     void
 */
void kmain(uint32_t boot_magic,
           struct multiboot_info *boot_record)
{
    // Panic if either the Multiboot magic number is wrong, or if there's no
    // Multiboot info record (this is done after
    PANIC_IF(boot_magic != MB_BOOT_MAGIC,
             "kmain not called with correct multiboot magic");
    PANIC_IF(boot_record == NULL,
             "NULL multiboot info record passed to kmain");

    // Load our GDT and initialize interrupts
    gdt_init();

    //int_disable();
    int_init();
    //int_disable();
    // Initialize the system timer
    timer_init(DEFAULT_SYSTEM_TIMER_FREQ);
    int_enable();

    // Loop forever to allow interrupts to fire
    while (true);
}
