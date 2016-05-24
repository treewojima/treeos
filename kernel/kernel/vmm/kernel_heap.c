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

#include <kernel/vmm/heap.h>

#include <kernel/debug.h>
#include <kernel/interrupt.h>
#include <kernel/multiboot.h>
#include <kernel/pmm.h>
#include <kernel/vmm/addr.h>
#include <string.h>

// NOTE: Right now, the kernel heap does NOT reclaim freed memory!
//       Consider using a linked list of blocks, perhaps with the linked list
//       information encoded into the head of the page to avoid the "allocating
//       memory while unable to allocate memory" problem. Either that or
//       customize the malloc implementation to mesh directly onto the PMM/VMM

// Kernel heap structure
static struct heap kheap;

// Interrupt reentry flag
static bool enable_int;

void kernel_heap_init(void)
{
    PANIC_IF(!heap_alloc(&kheap, KHEAP_START, 0),
             "failed to create kernel heap");
}

int liballoc_lock(void)
{
    // Just disable interrupts - this will need to be fixed with a proper
    // synchronization method later
    if (int_enabled())
    {
        enable_int = true;
        int_disable();
    }
    else
    {
        enable_int = false;
    }

    return 0;
}

int liballoc_unlock(void)
{
    // Just re-enable interrupts if necessary - again, needs to be fixed later
    if (enable_int)
    {
        int_enable();
    }

    return 0;
}

void *liballoc_alloc(size_t pages)
{
    // NOTE: This manually manipulates the heap for now, which is a bad idea

    if (!pages)
    {
        worry("allocating zero pages");
        return NULL;
    }

    // Preserve the starting address of this block
    uint32_t start = kheap.base + kheap.size;

    uint32_t vaddr = 0;
    struct page_table_entry pte;
    for ( ; pages > 0; pages--)
    {
        // Grab a free physical page
        memset(&pte, 0, sizeof(pte));
        PANIC_IF(!page_alloc(&pte), "out of memory");

        // Map it into the correct virtual address into the heap
        vaddr = kheap.base + kheap.size;
        page_map(pte.address << 12,
                 vaddr,
                 true, false);

        // Update the heap size
        kheap.size += PAGE_SIZE;
        PANIC_IF(kheap.size > KHEAP_MAX_SIZE,
                 "maximum kernel heap size exceeded");
    }

    return (void *)start;
}

int liballoc_free(void *block, size_t pages)
{
    KASSERT(block);
    KASSERT(IS_PAGE_ALIGNED(block));

    if (!pages)
    {
        worry("freeing zero pages");
        return -1;
    }

    PANIC_IF((uint32_t)block > kheap.base + kheap.size,
             "freeing block outside range of kernel heap");

    // NOTE: For now, don't bother worrying about heap fragmentation;
    //       just unmap and free the physical page
    uint32_t base = (uint32_t)block;
    uint32_t vaddr = 0, paddr = 0;
    for ( ; pages > 0; pages--)
    {
        vaddr = base + ((pages - 1) << PAGE_SIZE_SHIFT);

        paddr = (uint32_t)virt_to_phys((void *)vaddr);
        struct page_table_entry pte = { 0 };
        pte.address = paddr >> 12;

        page_unmap(vaddr);
        page_free(&pte);
    }

    return 0;
}
