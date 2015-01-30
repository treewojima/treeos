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

#if 0
static void map_page(uint32_t physaddr,
                     uint32_t virtualaddr,
                     bool rw,
                     bool user);
#endif

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
    //mm_init_kernel_heap();

    // Initialize the system timer and keyboard driver
    timer_init(DEFAULT_SYSTEM_TIMER_FREQ);
    kbd_init();

    int_enable();

    while (true);
}

#if 0
void map_page(uint32_t physaddr,
              uint32_t virtualaddr,
              bool rw,
              bool user)
{
    PANIC_IF((physaddr & 0xFFF) != 0, "physical address is not page-aligned");
    PANIC_IF((virtualaddr & 0xFFF) != 0, "virtual address is not page-aligned");

    uint32_t pgdir_index = (uint32_t)virtualaddr >> 22;
    uint32_t pgtbl_index = (uint32_t)virtualaddr >> 12 & 0x03FF;

    struct page_dir_entry *pde = (struct page_dir_entry *)0xFFFFF000;
    if (!pde[pgdir_index].present)
    {
        // Create a new page directory entry
        struct page_table_entry *new_pte = mm_placement_alloc(sizeof(*new_pte) * 1024, true);
        pde[pgdir_index].present = 1;
        pde[pgdir_index].rw = 1;
        pde[pgdir_index].user = 1;

        // Make sure the mapping is to the PTE's PHYSICAL address, not virtual
        uint32_t new_pte_physaddr = (uint32_t)mm_get_physaddr(new_pte);
        pde[pgdir_index].address = (uint32_t)new_pte_physaddr >> 12;
    }

    struct page_table_entry *pte =
            ((struct page_table_entry *)0xFFC00000) + (0x400 * pgdir_index);
    // TODO: this is debug-only, so fix this later
    if (pte[pgtbl_index].present)
    {
        char buffer[128];
        sprintf(buffer,
                "page table entry already  present for virtual address %x (mapped to physical address %x)",
                virtualaddr,
                physaddr);
        panic(buffer);
    }
    else
    {
        pte[pgtbl_index].present = 1;
        pte[pgtbl_index].rw = rw;
        pte[pgtbl_index].user = user;
        pte[pgtbl_index].address = physaddr >> 12;
    }

    // Update the TLB
    mm_flush_tlb(virtualaddr);
}
#endif
