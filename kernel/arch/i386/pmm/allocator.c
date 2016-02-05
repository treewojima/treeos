#include <arch/i386/pmm/allocator.h>

#include <arch/i386/pmm/paging.h>
#include <kernel/debug.h>
#include <string.h>

static uint32_t placement_heap;

void paging_allocator_init(void)
{
    // Set the placement heap to begin at the next page after the kernel tail
    extern uint8_t g_kernel_end;
    placement_heap = ((uint32_t)&g_kernel_end & 0xFFFFF000) + PAGE_SIZE;
}

void *paging_structure_alloc(void)
{
    KASSERT(PAGE_ALIGNED(placement_heap));

    // First, allocate a free page
    struct page_table_entry pte = { 0 };
    PANIC_IF(!page_alloc(&pte), "out of memory");

    // Map it to the next "slot" in the placement heap
    page_map(pte.address << 12,
             placement_heap,
             true, false);

    // Zero the entire page
    memset((void *)placement_heap, 0, PAGE_SIZE);

    uint32_t tmp = placement_heap;
    placement_heap += PAGE_SIZE;
    return (void *)tmp;
}
