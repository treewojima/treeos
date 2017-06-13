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

#include <arch/i386/pmm/paging.h>

#include <arch/i386/pmm/allocator.h>
#include <arch/i386/pmm/tlb.h>
#include <kernel/debug.h>
#include <kernel/vmm/addr.h>
#include <kernel/vmm/heap.h>
#include <kernel/util/bitmap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern struct page_dir_entry g_real_kernel_page_dir;
struct page_dir_entry *g_kernel_page_dir = &g_real_kernel_page_dir;

extern struct page_table_entry g_real_kernel_page_table;
struct page_table_entry *g_kernel_page_table = &g_real_kernel_page_table;

static uint32_t page_count;

static struct bitmap page_bitmap;
// MAXIMUM OF 128 MiB MEMORY SUPPORTED WITH THIS ARRAY
#define BITMAP_ARRAY_SIZE 1024
static unsigned page_bitmap_bits[BITMAP_ARRAY_SIZE];

uint32_t paging_init_bitmap(uint32_t mem_size)
{
    page_count = mem_size >> PAGE_SIZE_SHIFT;
    if (mem_size % PAGE_SIZE) page_count++;

    // Initialize bitmap by hand, since we don't have dynamic memory allocation
    page_bitmap.bits = page_bitmap_bits;
    page_bitmap.word_count = page_count / BITS_PER_WORD;
    if (page_count % BITS_PER_WORD) page_bitmap.word_count++;
    KASSERT(page_bitmap.word_count < BITMAP_ARRAY_SIZE);
    page_bitmap.flags = 0;

    // Set all of memory as "in use" by default
    uint32_t bytes_allocated = page_bitmap.word_count * sizeof(page_bitmap.bits);
    memset(page_bitmap.bits, 0xFF, BITMAP_ARRAY_SIZE); //bytes_allocated);

    return bytes_allocated;
}

void paging_init_region(uint32_t base, uint32_t size)
{
    base >>= PAGE_SIZE_SHIFT;
    uint32_t size_bits = size >> PAGE_SIZE_SHIFT;
    if (size % PAGE_SIZE) size_bits++;

    for (; (int)size_bits >= 0; base++, size_bits--)
    {
        bitmap_clear_bit(&page_bitmap, base);
    }

    // Ensure that allocations can't return zero
    bitmap_set_bit(&page_bitmap, 0);
}

void paging_deinit_region(uint32_t base, uint32_t size)
{
    base >>= PAGE_SIZE_SHIFT;
    uint32_t size_bits = size >> PAGE_SIZE_SHIFT;
    if (size % PAGE_SIZE) size_bits++;

    for (; (int)size_bits >= 0; base++, size_bits--)
    {
        bitmap_set_bit(&page_bitmap, base);
    }
}

struct page_table_entry *page_alloc(struct page_table_entry *page)
{
    KASSERT(page);

    uint32_t bit = bitmap_first_free(&page_bitmap);
    if (bit == (uint32_t)-1)
    {
        return NULL;
    }

    bitmap_set_bit(&page_bitmap, bit);

    // Note that because of the offset of the address field in a PTE, if the
    // page size is 4096, the shifting operations cancel each other out. This
    // will not be the case if PAGE_SIZE is different though
#if PAGE_SIZE != 4096
    page->address = bit * PAGE_SIZE >> 12;
#else
    page->address = bit;
#endif

    return page;
}

void page_free(struct page_table_entry *page)
{
    KASSERT(page);

#if PAGE_SIZE != 4096
    uint32_t bit = (page->address << 12) / PAGE_SIZE;
#else
    uint32_t bit = page->address;
#endif

    WORRY_IF(!bitmap_test_bit(&page_bitmap, bit), "clearing unset bit");
    bitmap_clear_bit(&page_bitmap, bit);
}

void page_map(uint32_t physaddr,
              uint32_t virtualaddr,
              bool rw,
              bool user)
{
    PANIC_IF(!IS_PAGE_ALIGNED(physaddr),
             "physical address is not page-aligned");
    PANIC_IF(!IS_PAGE_ALIGNED(virtualaddr),
             "virtual address is not page-aligned");
    PANIC_IF(!bitmap_test_bit(&page_bitmap, physaddr >> PAGE_SIZE_SHIFT),
             "mapping page that is not allocated in page_bitmap");

    uint32_t pgdir_index = (uint32_t)virtualaddr >> 22;
    uint32_t pgtbl_index = (uint32_t)virtualaddr >> 12 & 0x03FF;

    struct page_dir_entry *pde = (struct page_dir_entry *)0xFFFFF000;
    if (!pde[pgdir_index].present)
    {
        // Create a new page directory entry
        pde[pgdir_index].present = 1;
        pde[pgdir_index].rw = 1;
        pde[pgdir_index].user = 1;

        // Make sure the mapping is to the PTE's PHYSICAL address, not virtual
        void *table = paging_structure_alloc();
        pde[pgdir_index].address = (uint32_t)virt_to_phys(table) >> 12;
    }

    struct page_table_entry *pte =
            ((struct page_table_entry *)0xFFC00000) + (0x400 * pgdir_index);
    // TODO: this is debug-only, so fix this later
    if (pte[pgtbl_index].present)
    {
        char buffer[128];
        sprintf(buffer,
                "page table entry already present for virtual address %x (mapped to physical address %x)",
                virtualaddr,
                pte[pgtbl_index].address << 12);
        panic(buffer);
    }
    else
    {
        pte[pgtbl_index].present = 1;
        pte[pgtbl_index].rw = rw;
        pte[pgtbl_index].user = user;
        pte[pgtbl_index].address = physaddr >> 12;
    }

    flush_tlb(virtualaddr);
}

void page_unmap(uint32_t virtualaddr)
{
    PANIC_IF(!IS_PAGE_ALIGNED(virtualaddr),
             "virtual address is not page-aligned");

    uint32_t pgdir_index = (uint32_t)virtualaddr >> 22;
    uint32_t pgtbl_index = (uint32_t)virtualaddr >> 12 & 0x03FF;

    struct page_dir_entry *pde = (struct page_dir_entry *)0xFFFFF000;
    if (!pde[pgdir_index].present)
    {
        // How can we unmap something that doesn't even have an entry in the
        // page directory?
        char buffer[128];
        sprintf(buffer,
                "page directory entry not present for virtual address %x\n",
                virtualaddr);
        panic(buffer);
    }

    struct page_table_entry *pte =
            ((struct page_table_entry *)0xFFC00000) + (0x400 * pgdir_index);
    if (!pte[pgtbl_index].present)
    {
        // No entry for this page
        char buffer[128];
        sprintf(buffer,
                "page table entry not present for virtual address %x\n",
                virtualaddr);
        panic(buffer);
    }

    // It's like it was never there...
    memset(&pte[pgtbl_index], 0, sizeof(*pte));
    flush_tlb(virtualaddr);
}
