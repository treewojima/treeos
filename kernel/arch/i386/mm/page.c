#include <kernel/mm.h>

#include <kernel/debug.h>
#include <kernel/panic.h>
#include <stdio.h>
#include <string.h>

#define BITS_PER_BITMAP (8 * sizeof(*page_bitmap))

static uint32_t page_count, *page_bitmap;

static void set_bit(uint32_t bit);
static void clear_bit(uint32_t bit);
static bool test_bit(uint32_t bit);
static uint32_t first_free(void);

uint32_t mm_init_page_bitmap(uint32_t mem_size)
{    
    page_count = mem_size / PAGE_SIZE;
    if (mem_size % PAGE_SIZE) page_count++;

    uint32_t num_elements = page_count / BITS_PER_BITMAP;
    if (page_count % BITS_PER_BITMAP) num_elements++;

    page_bitmap = mm_placement_alloc(num_elements, false, false);

    // Set all of memory as "in use" by default
    //uint32_t bytes_allocated =
    //        frame_count / (BITS_PER_BITMAP / sizeof(*frame_bitmap));
    uint32_t bytes_allocated = num_elements * sizeof(*page_bitmap);
    memset(page_bitmap, 0xFF, bytes_allocated);

    return bytes_allocated;
}

void mm_init_region(uint32_t base, uint32_t size)
{
    base /= PAGE_SIZE;
    size /= PAGE_SIZE;

    while ((int)size >= 0)
    {
        clear_bit(base++);
        size--;
    }

    // Ensure that allocations can't return zero
    set_bit(0);
}

void mm_deinit_region(uint32_t base, uint32_t size)
{
    base /= PAGE_SIZE;
    size /= PAGE_SIZE;

    while ((int)size >= 0)
    {
        set_bit(base++);
        size--;
    }
}

struct page_table_entry *mm_alloc_page(struct page_table_entry *page)
{
    KASSERT(page);

    uint32_t address = first_free();
    if (address == (uint32_t)-1)
    {
        return NULL;
    }

    set_bit(address);
    address *= PAGE_SIZE;

    page->address = address >> 12;
    return page;
}

void mm_free_page(struct page_table_entry *page)
{
    KASSERT(page);

    uint32_t bit = (page->address << 12) / PAGE_SIZE;
    WORRY_IF(!test_bit(bit), "clearing unset bit");
    clear_bit(bit);
}

void mm_map_page(uint32_t physaddr,
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
        struct page_table_entry *new_pte =
                mm_placement_alloc(sizeof(*new_pte) * 1024, true, true);
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
    if (flush_tlb) mm_flush_tlb(virtualaddr);
}

static void set_bit(uint32_t bit)
{
    page_bitmap[bit / BITS_PER_BITMAP] |= (1 << (bit % BITS_PER_BITMAP));
}

static void clear_bit(uint32_t bit)
{
    page_bitmap[bit / BITS_PER_BITMAP] &= ~(1 << (bit % BITS_PER_BITMAP));
}

static bool test_bit(uint32_t bit)
{
    return page_bitmap[bit / BITS_PER_BITMAP] & (1 << (bit % BITS_PER_BITMAP));
}

static uint32_t first_free(void)
{
    for (uint32_t i = 0; i < page_count / BITS_PER_BITMAP; i++)
    {
        if (page_bitmap[i] != (uint32_t)-1)
        {
            for (uint32_t j = 0; j < BITS_PER_BITMAP; j++)
            {
                if (!(page_bitmap[i] & (1 << j)))
                {
                    return i * BITS_PER_BITMAP + j;
                }
            }
        }
    }

    return (uint32_t)-1;
}
