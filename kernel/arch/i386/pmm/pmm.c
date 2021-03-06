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

#include <arch/i386/pmm/pmm.h>

#include <arch/i386/cpu.h>
#include <arch/i386/pmm/allocator.h>
#include <arch/i386/pmm/paging.h>
#include <kernel/const.h>
#include <kernel/interrupt.h>
#include <kernel/multiboot.h>
#include <kernel/panic.h>
#include <stdio.h>
#include <string.h>

#ifndef USE_BOOTSTRAP_PAGING
//extern uint32_t g_placement_address;
extern uint8_t KERNEL_BOOT_VMA, KERNEL_HIGH_VMA;

static struct page_dir_entry kernel_pde[1024] ALIGNED(PAGE_SIZE);
static struct page_table_entry kernel_pte[1024] ALIGNED(PAGE_SIZE);

// Re-initialize paging (not used right now
static void reinit_paging(void);
#endif

static void parse_memory_map(const struct multiboot_info *const mbi);
static void preserve_modules(const struct multiboot_info *const mbi);
static void page_fault_int_handler(const struct thread_context *const context);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PUBLIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Initialize physical memory manager
 * Parameters:
 *     none
 *
 * Returns:
 *     void
 */
void pmm_init(void)
{
    // Make sure we can use GRUB's memory information
    extern const struct multiboot_info g_multiboot_info;
    const struct multiboot_info *const mbi = &g_multiboot_info;
    PANIC_IF(!(mbi->flags & MULTIBOOT_INFO_MEMORY),
             "Multiboot header does not include memory size information");
    PANIC_IF(!(mbi->flags & MULTIBOOT_INFO_MEM_MAP),
             "Multiboot header does not include memory map information");

    printf("mem_lower = %u KiB\n", mbi->mem_lower);
    printf("mem_upper = %u KiB\n", mbi->mem_upper);
    printf("mem_total = %u KiB\n", mbi->mem_lower + mbi->mem_upper);

    // Initialize physical memory bitmap
    paging_init_bitmap((mbi->mem_lower + mbi->mem_upper) * 1024);

    // Loop through the memory map that GRUB provides from the BIOS
    parse_memory_map(mbi);

    // Make sure the kernel's location in physical memory isn't marked as free
    uint32_t real_kernel_start = KERN_START - KERN_HIGH_OFFSET;
    uint32_t real_kernel_end = KERN_END - KERN_HIGH_OFFSET;
    paging_deinit_region(real_kernel_start, real_kernel_end - real_kernel_start);

    // Also make sure that any modules passed through GRUB are preserved
    preserve_modules(mbi);

#ifndef USE_BOOTSTRAP_PAGING
    reinit_paging();
#endif

    // Initialize the paging structure allocator
    paging_allocator_init();

    // Set up our page fault handler
    int_register_handler(14, page_fault_int_handler);

    printf("[mm] paging enabled\n");
}

#if 0
void dump_pgdir(void)
{
    struct page_dir_entry *pde = (struct page_dir_entry *)0xFFFFF000;
    for (int i = 0; i < 1024; i++)
    {
        if (!pde[i].present) continue;

        pde[i].a
        for (int j = 0; j < 1024; j++)
        {

        }
    }
}
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  STATIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static void parse_memory_map(const struct multiboot_info *const mbi)
{
    struct multiboot_mmap_entry *mmap =
            (struct multiboot_mmap_entry *)mbi->mmap_addr;
    while ((uint32_t)mmap < mbi->mmap_addr + mbi->mmap_length)
    {
        uint32_t addr32 = (uint32_t)(mmap->addr & 0xFFFFFFFF);
        uint32_t len32 = (uint32_t)(mmap->addr & 0xFFFFFFFF);

        printf("memory region from %p to %p is %s\n",
               addr32,
               addr32 + len32,
               mmap->type == MULTIBOOT_MEMORY_AVAILABLE ? "available" : "reserved");

        // Only consider the region if it's marked as available
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
        {
            paging_init_region(addr32, len32);
        }

        mmap = (struct multiboot_mmap_entry *)
                ((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }
}

static void preserve_modules(const struct multiboot_info *const mbi)
{
    // If there's a positive module count but no module information flag set,
    // then something's very wrong
    PANIC_IF(mbi->mods_count && !(mbi->flags & MULTIBOOT_INFO_MODS),
             "Multiboot header does not include module information");

    // Loop through each module and mark its memory location as unavailable
    struct multiboot_mod_list *mod =
            (struct multiboot_mod_list *)mbi->mods_addr;
    for (uint32_t i = 0; i < mbi->mods_count; i++, mod++)
    {
        KASSERT(mod);

        printf("module found in region from %p to %p\n",
               mod->mod_start,
               mod->mod_end);

        paging_deinit_region(mod->mod_start, mod->mod_end - mod->mod_start - 1);
    }
}

static void page_fault_int_handler(const struct thread_context *const context)
{
    char msg_buf[MAX_PANIC_BUF];
    sprintf(msg_buf, "page fault at address %p", read_cr2());
    panic_r(msg_buf, context);
}

#ifndef USE_BOOTSTRAP_PAGING
static void reinit_paging(void)
{
    // Identity map the first 4 MiB of physical memory
    //struct page_table_entry *pte = mm_placement_alloc(sizeof(*pte) * 1024, true);
    struct page_table_entry *pte = kernel_pte;
    memset(pte, 0, sizeof(*pte) * 1024);
    // Skip the zero page
    for (int i = 1; i < 1024; i++)
    {
        pte[i].present = 1;
        pte[i].rw = 1;
        pte[i].address = (i * PAGE_SIZE) >> 12;
    }

    // Create and initialize a new page directory
    //struct page_dir_entry *pde = mm_placement_alloc(sizeof(*pde) * 1024, true);
    struct page_dir_entry *pde = kernel_pde;
    memset(pde, 0, sizeof(*pde) * 1024);

    // Insert into pde as an identity mapping
    pde->present = 1;
    pde->rw = 1;
    pde->address = (uint32_t)mm_get_physaddr(pte) >> 12;

    // Also map as "higher half" at the 3 GiB mark
    uint32_t higher_half_page =
            ((uint32_t)&KERNEL_BOOT_VMA + (uint32_t)&KERNEL_HIGH_VMA) >> 22;
    pde[higher_half_page].present = 1;
    pde[higher_half_page].rw = 1;
    pde[higher_half_page].address = (uint32_t)mm_get_physaddr(pte) >> 12;

#if 0
    // Map the rest of available memory
    int pages = 0;
    for (uint32_t i = 1; i < 1023; i++)
    {
        // Skip the spot occupied by the higher half kernel
        if (i == higher_half_page) continue;

        pte = mm_placement_alloc(sizeof(*pte) * 1024, true);

        for (int j = 0; j < 1024; j++, pages++)
        {
            if (mm_alloc_frame(&pte[j]) == NULL)
                break;
            pte[j].present = 1;
            pte[j].rw = 1;
            pte[j].user = 1;
        }

        pde[i].present = 1;
        pde[i].rw = 1;
        pde[i].user = 1;
        pde[i].address = (uint32_t)mm_get_physaddr(pte) >> 12;
    }
#endif

    // Map the last entry of the PDE to itself
    pde[1023].present = 1;
    pde[1023].rw = 1;
    pde[1023].address = (uint32_t)mm_get_physaddr(pde) >> 12;

    // Switch to the new paging setup
    mm_flush_tlb_full();
    write_cr3((uint32_t)mm_get_physaddr(pde));
    write_cr0(read_cr0() | 0x80000000);
}
#endif
