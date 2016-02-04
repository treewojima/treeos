#include <arch/i386/pmm/paging.h>

#include <arch/i386/pmm/tlb.h>
#include <kernel/debug.h>
#include <kernel/vmm/addr.h>
#include <kernel/vmm/heap.h>
#include <kernel/util/bitmap.h>
#include <stdio.h>
#include <string.h>

static uint32_t page_count;
static struct bitmap page_bitmap;

uint32_t paging_init_bitmap(uint32_t mem_size)
{    
    page_count = mem_size >> PAGE_SIZE_SHIFT;
    if (mem_size % PAGE_SIZE) page_count++;

    bitmap_alloc(&page_bitmap, page_count);

    // Set all of memory as "in use" by default
    //uint32_t bytes_allocated =
    //        frame_count / (BITS_PER_BITMAP / sizeof(*frame_bitmap));
    uint32_t bytes_allocated = page_bitmap.word_count * sizeof(page_bitmap.bits);
    memset(page_bitmap.bits, 0xFF, bytes_allocated);

    return bytes_allocated;
}

void paging_init_region(uint32_t base, uint32_t size)
{
    base >>= PAGE_SIZE_SHIFT;
    size >>= PAGE_SIZE_SHIFT;

    for (; (int)size >= 0; base++, size--)
    {
        bitmap_clear_bit(&page_bitmap, base);
    }

    // Ensure that allocations can't return zero
    bitmap_set_bit(&page_bitmap, 0);
}

void paging_deinit_region(uint32_t base, uint32_t size)
{
    base >>= PAGE_SIZE_SHIFT;
    size >>= PAGE_SIZE_SHIFT;

    for (; (int)size >= 0; base++, size--)
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
              bool user,
              bool flush)
{
    PANIC_IF((physaddr & 0xFFF) != 0,
             "physical address is not page-aligned");
    PANIC_IF((virtualaddr & 0xFFF) != 0,
             "virtual address is not page-aligned");
    PANIC_IF(!bitmap_test_bit(&page_bitmap, physaddr >> PAGE_SIZE_SHIFT),
             "mapping page that is not allocated in page_bitmap");

    uint32_t pgdir_index = (uint32_t)virtualaddr >> 22;
    uint32_t pgtbl_index = (uint32_t)virtualaddr >> 12 & 0x03FF;

    struct page_dir_entry *pde = (struct page_dir_entry *)0xFFFFF000;
    if (!pde[pgdir_index].present)
    {
        // Create a new page directory entry
        struct page_table_entry *new_pte =
                kcalloca(1024, sizeof(*new_pte));
        pde[pgdir_index].present = 1;
        pde[pgdir_index].rw = 1;
        pde[pgdir_index].user = 1;

        // Make sure the mapping is to the PTE's PHYSICAL address, not virtual
        uint32_t new_pte_physaddr = (uint32_t)virt_to_phys(new_pte);
        pde[pgdir_index].address = (uint32_t)new_pte_physaddr >> 12;
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

    // Update the TLB
    if (flush) flush_tlb(virtualaddr);
}
