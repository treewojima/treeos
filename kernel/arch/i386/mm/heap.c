#include <kernel/mm.h>

#include <kernel/debug.h>
#include <kernel/multiboot.h>
#include <stddef.h>
#include <stdio.h>

// Place the default kernel heap at 32 MiB in memory and 32 MiB in size
// NOTE: This is just temporary
#define DEFAULT_KERNEL_HEAP_LOC  0x2000000
#define DEFAULT_KERNEL_HEAP_SIZE 0x2000000

uint32_t g_placement_address;

static void map_page(uint32_t physaddr,
                     uint32_t virtualaddr,
                     bool rw,
                     bool user,
                     bool flush_tlb);

void mm_init_placement_heap(void)
{
    // Start off with the kernel heap at the end of the kernel image
    extern const uint8_t g_kernel_end;
    g_placement_address = (uint32_t)&g_kernel_end;

    // If there are multiboot modules present, take them into account. Otherwise
    // they'll be counted as free heap space, which would lead to... undefined
    // behavior.
    // TODO: Add module support
    extern const struct multiboot_info g_multiboot_info;
    KASSERT(g_multiboot_info.mods_count == 0);
}

void mm_init_kernel_heap(void)
{
    uint32_t start_addr = DEFAULT_KERNEL_HEAP_LOC;
    uint32_t size = DEFAULT_KERNEL_HEAP_SIZE;

    struct page_table_entry pte;
    for (uint32_t addr = start_addr; addr < start_addr + size; addr += PAGE_SIZE)
    {
        PANIC_IF(!mm_alloc_frame(&pte), "out of memory");
        map_page(pte.address << 12, addr, true, false, false);
    }

    mm_flush_tlb_full();

    printf("[mm] initialized kernel heap (%d pages/%u KiB)\n",
           size / PAGE_SIZE,
           size / 1024);
}

void *mm_placement_alloc(uint32_t size, bool align)
{
    // Align the placement address if it's not already
    if (align && (g_placement_address & 0xFFFFF000))
    {
        g_placement_address &= 0xFFFFF000;
        g_placement_address += 0x1000;
    }

    uint32_t tmp = g_placement_address;
    g_placement_address += size;
    return (void *)tmp;
}

void map_page(uint32_t physaddr,
              uint32_t virtualaddr,
              bool rw,
              bool user,
              bool flush_tlb)
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
    if (flush_tlb)
        mm_flush_tlb(virtualaddr);
}
