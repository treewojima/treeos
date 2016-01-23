#include <arch/i386/cpu.h>
#include <arch/i386/serio.h>
#include <kernel/interrupt.h>
#include <kernel/kbd.h>
#include <kernel/mm.h>
#include <kernel/multiboot.h>
#include <kernel/panic.h>
#include <kernel/reboot.h>
#include <kernel/timer.h>
#include <kernel/tty.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void test_map_page(void);

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

    // Update various addresses in the struct to reflect their virtual,
    // higher-half counterparts
#if 0
    extern uint8_t KERNEL_HIGH_VMA;
    uint32_t offset = (uint32_t)&KERNEL_HIGH_VMA;
    mbi->boot_loader_name += offset;
    mbi->cmdline += offset;
    mbi->mmap_addr += offset;
    mbi->mods_addr += (mbi->mods_addr ? offset : 0);
    mbi->syms.elf.addr += offset; // updates syms.aout as well
    mbi->vbe_control_info += offset;
    mbi->vbe_mode_info += offset;
#endif

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

    //for (;;) ;
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

    // Initialize paging and the proper kernel heap
    mm_init();
    int_enable();
    test_map_page();

    panic("end of kmain");
}

void test_map_page(void)
{
    printf("testing map_page\n");

    struct page_table_entry pte = { 0 };
    PANIC_IF(!mm_alloc_page(&pte), "out of memory");

    const uint32_t vaddr = 768 * 1024 * 1024;

    uint32_t paddr = pte.address << 12;
    mm_map_page(paddr, vaddr, true, true, true);

    int *test = (int *)vaddr;
    //*test = 69;
    printf("*test      = %d\n", *test);
    printf("test_vaddr = %p\n", test);
    printf("test_paddr = %p\n", mm_get_physaddr(test));

    mm_free_page(&pte);

    printf("done testing map_page\n");
}
