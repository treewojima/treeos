#include <arch/i386/cpu.h>
#include <arch/i386/serio.h>
#include <kernel/interrupt.h>
#include <kernel/kbd.h>
#include <kernel/mm.h>
#include <kernel/multiboot.h>
#include <kernel/panic.h>
#include <kernel/timer.h>
#include <kernel/tty.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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
    PANIC_IF(boot_magic != MB_BOOT_MAGIC,
             "kinit not called with correct multiboot magic");
    PANIC_IF(mbi == NULL,
             "NULL multiboot info record passed to kinit");

    // Initialize the (very primitive) serial and terminal drivers
    serio_init(SERIO_COM1);
    tty_init();

    // Initialize the kernel placement heap
    mm_init_heap(mbi);

    if (mbi->flags & MB_FLAG_MMAP)
    {
        struct multiboot_memory_map *mmap =
                (struct multiboot_memory_map *)mbi->mmap_addr;
        while ((uint32_t)mmap < mbi->mmap_addr + mbi->mmap_length)
        {
            printf("memory from %p to %p is %s\n",
                   mmap->base_addr_low,
                   mmap->base_addr_low + mmap->length_low,
                   (mmap->type == 1 ? "available" : "reserved"));

            mmap = (struct multiboot_memory_map *)
                    ((uint32_t)mmap + mmap->size + sizeof(mmap->size));
        }
    }
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

    mm_init();

    // Initialize the system timer and keyboard driver
    timer_init(DEFAULT_SYSTEM_TIMER_FREQ);
    //kbd_init();

    int_enable();

    while (true);
}
