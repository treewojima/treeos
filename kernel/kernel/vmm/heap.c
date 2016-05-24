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
#include <kernel/pmm.h>
#include <stdio.h>
#include <stdlib.h>

#define MUST_FREE (1 << 0)

struct heap *heap_alloc(struct heap *heap, uint32_t base, uint32_t size)
{
    KASSERT(base);
    //KASSERT(size);

    // Make sure the heap is page-aligned
    if (base % PAGE_SIZE || size % PAGE_SIZE)
    {
        char buf[100];
        sprintf(buf, "heap %p not page-aligned: base=%u, size=%u", heap, base, size);
        panic(buf);
    }

    if (!heap)
    {
#ifdef PLACEHOLDER_FOR_WORKING_MALLOC
        heap = malloc(sizeof(struct heap));
        memset(heap, 0, sizeof(struct heap));
        heap->flags |= MUST_FREE;
#else
        heap = kcalloc(1, sizeof(struct heap));
#endif
    }
    KASSERT(heap);

    heap->base = base;
    heap->size = size;

    return heap;
}

// NOTE: This does not allocate consecutive pages, and has to be revised!
void *heap_grow(struct heap *heap, uint32_t pages)
{
    // Squelch compiler warnings/errors
    if (heap) heap = 0;
    if (pages) pages = 0;
    panic("THIS FUNCTION IS UNIMPLEMENTED AND SHOULD NOT BE USED");
    return NULL;

#if 0
    KASSERT(heap);

    // Cause for slight alarm, but not disastrous
    if (!pages)
    {
        char buf[100];
        sprintf(buf, "growing heap %p by zero pages", heap);
        worry(buf);
        return NULL;
    }

    // Make sure the heap is (still) page-aligned
    if (heap->base % PAGE_SIZE || heap->size % PAGE_SIZE)
    {
        char buf[100];
        sprintf(buf, "heap %p not page-aligned: base=%u, limit=%u",
                heap, heap->base, heap->size);
        panic(buf);
    }

    // Allocate pages
    struct page_table_entry *pte = NULL;
    for (; pages; pages--)
    {
        // First, grab a free page
        PANIC_IF(!page_alloc(pte), "out of memory");

        // Next, find a free spot in the heap bitmap
        unsigned bit = bitmap_first_free(&heap->bitmap);
        PANIC_IF(bit == (unsigned)-1, "heap bitmap full");
        uint32_t offset = bit << PAGE_SIZE_SHIFT;

        // Make sure we aren't growing past the defined heap size
        if (offset >= heap->size)
        {
            char buf[100];
            sprintf(buf, "attempted to grow heap %p past defined size: "
                         "offset=%u, size=%u", heap, offset, heap->limit);
            worry(buf);
            return NULL;
        }

        // Map to the appropriate virtual address
        // NOTE: Come back to this later and evaluate flags for user space!
        page_map(pte->address << 12,
                 heap->base + offset,
                 true, false, true);
        bitmap_set_bit(&heap->bitmap, bit);
    }

    return heap;
#endif
}

void heap_shrink(struct heap *heap, void *ptr, uint32_t pages)
{
    // Squelch compiler warnings/errors
    if (heap) heap = 0;
    if (ptr) ptr = 0;
    if (pages) pages = 0;
    panic("THIS FUNCTION IS UNIMPLEMENTED AND SHOULD NOT BE USED");
}
