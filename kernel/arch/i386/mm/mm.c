#include <kernel/mm.h>

#include <arch/i386/cpu.h>
#include <kernel/debug.h>
#include <kernel/interrupt.h>
#include <kernel/multiboot.h>
#include <kernel/reboot.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern uint32_t g_placement_address;

extern struct page_dir_entry g_kernel_page_dir[1024];
extern struct page_table_entry g_kernel_page_table[1024];

// Page fault handler
static void page_fault_int_handler(struct registers *registers);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PUBLIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Initialize paging and virtual memory
 * Parameters:
 *     none
 *
 * Returns:
 *     void
 */
void mm_init(void)
{
    // Make sure we can use GRUB's memory information
    extern const struct multiboot_info g_multiboot_info;
    const struct multiboot_info *const mbi = &g_multiboot_info;
    PANIC_IF(!((mbi->flags & MB_FLAG_MEM)),
             "Multiboot header does not include memory size information");
    PANIC_IF(!((mbi->flags & MB_FLAG_MMAP)),
             "Multiboot header does not include memory map information");

    printf("mem_lower = %d KiB\n", mbi->mem_lower);
    printf("mem_upper = %d KiB\n", mbi->mem_upper);
    printf("mem_total = %d KiB\n", mbi->mem_lower + mbi->mem_upper);

    // Initialize physical memory map
    mm_init_frame_bitmap((mbi->mem_lower + mbi->mem_upper) * 1024);

    // Loop through the memory map that GRUB provides from the BIOS
    struct multiboot_memory_map *mmap =
            (struct multiboot_memory_map *)mbi->mmap_addr;
    while ((uint32_t)mmap < mbi->mmap_addr + mbi->mmap_length)
    {
        printf("memory region from %p to %p is %s\n",
               mmap->base_addr_low,
               mmap->base_addr_low + mmap->length_low,
               mmap->type == 1 ? "available" : "reserved");

        // Only consider the region if it's marked as available
        if (mmap->type == 1)
        {
            mm_init_region(mmap->base_addr_low, mmap->length_low);
        }

        mmap = (struct multiboot_memory_map *)
                ((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }

    // Make sure the kernel's region of memory isn't marked as free
    // NOTE: Right now this just blindly allocates the first 4 MiB of memory
    mm_deinit_region(0, 4096 * 1024);

    struct page_table_entry *pte = g_kernel_page_table;
    memset(pte, 0, sizeof(*pte) * 1024);

    // Map the first 4 MiB of physical memory into the higher half (3 GiB)
    for (int i = 0; i < 1024; i++)
    {
        pte[i].present = 1;
        pte[i].rw = 1;
        pte[i].address = (i * PAGE_SIZE) >> 12;
    }

    extern uint8_t KERNEL_PHYS_ADDR;
    extern uint8_t KERNEL_VIRT_OFFSET;
    uint32_t higher_half_page =
            ((uint32_t)&KERNEL_PHYS_ADDR + (uint32_t)&KERNEL_VIRT_OFFSET) >> 22;

    g_kernel_page_dir[higher_half_page].present = 1;
    g_kernel_page_dir[higher_half_page].rw = 1;
    g_kernel_page_dir[higher_half_page].address = (uint32_t)pte >> 12;

#if 0
    int pages = 0;

    // Map the rest of available memory
    for (int i = 1; i < 1023; i++)
    {
        pte = mm_placement_alloc(sizeof(*pte) * 1024, true);

        for (int j = 0; j < 1024; j++, pages++)
        {
            if (mm_alloc_frame(&pte[j]) == NULL)
                break;
            pte[j].present = 1;
            pte[j].rw = 1;
            pte[j].user = 1;
        }

        g_kernel_page_dir[i].present = 1;
        g_kernel_page_dir[i].rw = 1;
        g_kernel_page_dir[i].user = 1;
        g_kernel_page_dir[i].address = (uint32_t)pte >> 12;
    }
#endif

    // Map the last entry of the PDE to itself
    g_kernel_page_dir[1023].present = 1;
    g_kernel_page_dir[1023].rw = 1;
    g_kernel_page_dir[1023].address =
            (uint32_t)mm_get_physaddr(g_kernel_page_dir) >> 12;

    // Enable paging
    //mm_flush_tlb_full();
    //write_cr3((uint32_t)&g_kernel_page_dir);
    write_cr3((uint32_t)mm_get_physaddr(g_kernel_page_dir));
    write_cr0(read_cr0() | 0x80000000);

    // Set up our page fault handler
    int_register_handler(14, page_fault_int_handler);

    //printf("[mm] paging enabled (%d pages/%u KiB free)\n", pages, pages * PAGE_SIZE / 1024);
    printf("[mm] paging enabled\n");
}

void *mm_get_physaddr(void *virtualaddr)
{
    uint32_t pgdir_index = (uint32_t)virtualaddr >> 22;
    uint32_t pgtbl_index = (uint32_t)virtualaddr >> 12 & 0x03FF;

    struct page_dir_entry *pde = (struct page_dir_entry *)0xFFFFF000;
    if (!pde[pgdir_index].present)
    {
        char msg[128];
        sprintf(msg,
                "virtual address %x references invalid page directory entry %u",
                (uint32_t)virtualaddr,
                pgdir_index);
        panic(msg);
    }

    struct page_table_entry *pte =
            ((struct page_table_entry *)0xFFC00000) + (0x400 * pgdir_index);
    if (!pte[pgtbl_index].present)
    {
        char msg[128];
        sprintf(msg,
                "virtual address %x references invalid page table entry %u in page directory %u",
                (uint32_t)virtualaddr,
                pgtbl_index,
                pgdir_index);
        panic(msg);
    }

    uint32_t *pte_int = (uint32_t *)&pte[pgtbl_index];
    return (void *)((*pte_int & ~0xFFF) + ((uint32_t)virtualaddr & 0xFFF));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  STATIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static void page_fault_int_handler(struct registers *registers)
{
    char msg_buf[MAX_PANIC_BUF];
    sprintf(msg_buf, "page fault at address %p", read_cr2());
    panic_r(msg_buf, registers);
}
