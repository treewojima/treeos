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

    // Copy the Multiboot info struct so it doesn't get overwritten
    // in memory later
    // TODO: do this for Multiboot modules as well!
    extern const struct multiboot_info g_multiboot_info;
    memcpy((struct multiboot_info *)&g_multiboot_info, mbi, sizeof(*mbi));

    // Initialize the (very primitive) serial and terminal drivers
    serio_init(SERIO_COM1);
    tty_init();

    // Initialize the kernel placement heap
    mm_init_placement_heap();
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

    // Initialize paging and the proper kernel heap
    mm_init();
    mm_init_kernel_heap();

    // Initialize the system timer and keyboard driver
    timer_init(DEFAULT_SYSTEM_TIMER_FREQ);
    kbd_init();

    int_enable();

#if 0
    struct page_table_entry pte = { 0 };
    PANIC_IF(!mm_alloc_frame(&pte), "out of memory");

    uint32_t real_physaddr = pte.address << 12;
    map_page(real_physaddr, (32 * 1024 * 1024), true, true);

    int *test = (int *)(32 * 1024 * 1024);
    *test = 69;
    printf("*test      = %d\n", *test);
    printf("test_vaddr = %p\n", test);
    printf("test_paddr = %p\n", mm_get_physaddr(test));
#endif

    for (;;) ;
}
